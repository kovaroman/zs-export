<?php

namespace ElasticExport\ES_Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;

class BilligerDE extends CSVGenerator
{
	/*
     * @var ElasticExportHelper
     */
    private $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

    /**
     * @var array
     */
    private $idlVariations = array();

	/**
     * Billiger constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ElasticExportHelper $elasticExportHelper, ArrayHelper $arrayHelper)
    {
        $this->elasticExportHelper = $elasticExportHelper;
		$this->arrayHelper = $arrayHelper;
    }

	/**
	 * @param array $resultList
	 */
	protected function generateContent($resultList, array $formatSettings = [])
	{
		if(is_array($resultList['documents']) && count($resultList['documents']) > 0)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(";");

			$this->addCSVContent([
					'aid',
					'name',
					'price',
					'link',
					'brand',
					'ean',
					'desc',
					'shop_cat',
					'image',
					'dlv_time',
					'dlv_cost',
					'ppu',
					'mpnr',

			]);

            $currentItemId = null;
            $previousItemId = null;
            $variations = array();

            //Create a List of all VariationIds
            $variationIdList = array();
            foreach($resultList['documents'] as $variation)
            {
                $variationIdList[] = $variation['id'];
            }

            //Get the missing fields in ES from IDL
            if(is_array($variationIdList) && count($variationIdList) > 0)
            {
                /**
                 * @var \ElasticExport\ES_IDL_ResultList\BilligerDE $idlResultList
                 */
                $idlResultList = pluginApp(\ElasticExport\ES_IDL_ResultList\BilligerDE::class);
                $idlResultList = $idlResultList->getResultList($variationIdList, $settings);
            }

            //Creates an array with the variationId as key to surpass the sorting problem
            if(isset($idlResultList) && $idlResultList instanceof RecordList)
            {
                $this->createIdlArray($idlResultList);
            }

            foreach($resultList['documents'] as $variation)
            {
                // Case first variation
                if ($currentItemId === null)
                {
                    $previousItemId = $variation['data']['item']['id'];
                }
                $currentItemId = $variation['data']['item']['id'];

                // Check if it's the same item
                if ($currentItemId == $previousItemId)
                {
                    $variations[] = $variation;
                }
                else
                {
                    $this->buildRows($settings, $variations);
                    $variations = array();
                    $variations[] = $variation;
                    $previousItemId = $variation['data']['item']['id'];
                }
            }
		}
	}

    /**
     * @param KeyValue $settings
     * @param array $variations
     */
	private function buildRows($settings, $variations)
    {
        foreach($variations as $variation)
        {
            $attributes = '';
            $attributeName = $this->elasticExportHelper->getEsAttributeName($variation, $settings, ',');
            $attributeValue = $this->elasticExportHelper->getEsAttributeValueSetShortFrontendName($variation, $settings, ',');
            if (strlen($attributeName) && strlen($attributeValue))
            {
                $attributes = $this->elasticExportHelper->getAttributeNameAndValueCombination($attributeName, $attributeValue);
            }

            if(strlen($attributes) <= 0 && count($variations) > 1)
            {
                continue;
            }

            $dlvCost = $this->elasticExportHelper->getEsShippingCost($variation['data']['item']['id'], $settings);

            if(!is_null($dlvCost))
            {
                $dlvCost = number_format((float)$dlvCost, 2, ',', '');
            }
            else
            {
                $dlvCost = '';
            }

            $data = [
                'aid' 		=> $variation['id'],
                'name' 		=> $this->elasticExportHelper->getEsName($variation, $settings) . (strlen($attributes) ? ', ' . $attributes : ''),
                'price' 	=> number_format((float)$this->idlVariations[$variation['id']]['variationRetailPrice.price'], 2, '.', ''),
                'link' 		=> $this->elasticExportHelper->getEsUrl($variation, $settings, true, false),
                'brand' 	=> $this->elasticExportHelper->getExternalManufacturerName((int)$variation['data']['item']['manufacturer']['id']),
                'ean' 		=> $this->elasticExportHelper->getEsBarcodeByType($variation, $settings->get('barcode')),
                'desc' 		=> $this->elasticExportHelper->getEsDescription($variation, $settings),
                'shop_cat' 	=> $this->elasticExportHelper->getCategory((int)$variation['data']['defaultCategories']['id'], $settings->get('lang'), $settings->get('plentyId')),
                'image'		=> $this->elasticExportHelper->getEsMainImage($variation, $settings),
                'dlv_time' 	=> $this->elasticExportHelper->getEsAvailability($variation, $settings, false),
                'dlv_cost' 	=> $dlvCost,
                'ppu' 		=> $this->elasticExportHelper->getEsBasePrice($variation, $this->idlVariations),
                'mpnr' 		=> $variation['data']['variation']['model'],
            ];

            $this->addCSVContent(array_values($data));
        }
    }

    /**
     * @param RecordList $idlResultList
     */
    private function createIdlArray($idlResultList)
    {
        if($idlResultList instanceof RecordList)
        {
            foreach($idlResultList as $idlVariation)
            {
                if($idlVariation instanceof Record)
                {
                    $this->idlVariations[$idlVariation->variationBase->id] = [
                        'itemBase.id' => $idlVariation->itemBase->id,
                        'variationBase.id' => $idlVariation->variationBase->id,
                        'variationRetailPrice.price' => $idlVariation->variationRetailPrice->price,
                    ];
                }
            }
        }
    }
}

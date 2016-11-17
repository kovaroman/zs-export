<?php

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;

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
	 * @param RecordList $resultData
	 */
	protected function generateContent($resultData, array $formatSettings = [])
	{
		if($resultData instanceof RecordList)
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

			foreach($resultData as $item)
			{
                $attributes = '';
                $attributeName = $this->elasticExportHelper->getAttributeName($item, $settings, ',');
                $attributeValue = $this->elasticExportHelper->getAttributeValueSetShortFrontendName($item, $settings, ',');
                if (strlen($attributeName) && strlen($attributeValue))
                {
                    $attributes = $this->elasticExportHelper->getAttributeNameAndValueCombination($attributeName, $attributeValue);
                }

                if(strlen($attributes) <= 0 && $item->itemBase->variationCount > 1)
                {
                    continue;
                }

                $dlvCost = $this->elasticExportHelper->getShippingCost($item, $settings);
                if(!is_null($dlvCost))
                {
                    $dlvCost = number_format($dlvCost, 2, ',', '');
                }
                else
                {
                    $dlvCost = '';
                }

				$data = [
					'aid' 		=> $item->variationBase->id,
					'name' 		=> $this->elasticExportHelper->getName($item, $settings) . (strlen($attributes) ? ', ' . $attributes : ''),
					'price' 	=> number_format($this->elasticExportHelper->getPrice($item), 2, '.', ''),
					'link' 		=> $this->elasticExportHelper->getUrl($item, $settings, true, false),
					'brand' 	=> $this->elasticExportHelper->getExternalManufacturerName($item->itemBase->producerId),
					'ean' 		=> $this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode')),
					'desc' 		=> $this->elasticExportHelper->getDescription($item, $settings),
	                'shop_cat' 	=> $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
					'image'		=> $this->elasticExportHelper->getMainImage($item, $settings),
					'dlv_time' 	=> $this->elasticExportHelper->getAvailability($item, $settings, false),
					'dlv_cost' 	=> $dlvCost,
	                'ppu' 		=> $this->elasticExportHelper->getBasePrice($item, $settings),
	                'mpnr' 		=> $item->variationBase->model,
				];

				$this->addCSVContent(array_values($data));
			}
		}
	}
}

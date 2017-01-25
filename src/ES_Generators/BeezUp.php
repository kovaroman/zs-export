<?php

namespace ElasticExport\ES_Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Item\Attribute\Contracts\AttributeValueNameRepositoryContract;
use Plenty\Modules\Item\Attribute\Models\AttributeValueName;

class BeezUp extends CSVGenerator
{

	const PROPERTY_TYPE_DESCRIPTION = 'description';

	/*
	 * @var ElasticExportHelper
	 */
    private $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
    private $arrayHelper;

	/**
	 * AttributeValueNameRepositoryContract $attributeValueNameRepository
	 */
    private $attributeValueNameRepository;

    /**
     * @var array $idlVariations
     */
    private $idlVariations = array();

    /**
     * Geizhals constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     * @param AttributeValueNameRepositoryContract $attributeValueNameRepository
     */
    public function __construct(
        ElasticExportHelper $elasticExportHelper,
        ArrayHelper $arrayHelper,
        AttributeValueNameRepositoryContract $attributeValueNameRepository
    )
    {
        $this->elasticExportHelper = $elasticExportHelper;
        $this->arrayHelper = $arrayHelper;
        $this->attributeValueNameRepository = $attributeValueNameRepository;
    }

    /**
     * @param array $resultData
     * @param array $formatSettings
     */
    protected function generateContent($resultData, array $formatSettings = [])
	{
		if(is_array($resultData) && count($resultData['documents']) > 0)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(";");

			$this->addCSVContent([
                'Produkt ID',
                'Artikel Nr',
                'MPN',
                'EAN',
                'Marke',
                'Produktname',
                'Produktbeschreibung',
                'Preis inkl. MwSt.',
                'UVP inkl. MwSt.',
                'Produkt-URL',
                'Bild-URL',
                'Bild-URL2',
                'Bild-URL3',
                'Bild-URL4',
                'Bild-URL5',
                'Lieferkosten',
                'Auf Lager',
                'Lagerbestand',
                'Lieferfrist',
                'Kategorie 1',
                'Kategorie 2',
                'Kategorie 3',
                'Kategorie 4',
                'Kategorie 5',
                'Kategorie 6',
                'Farbe',
                'Größe',
                'Gewicht',
                'Grundpreis',
                'ID'
			]);

            $variationIdList = array();
            foreach($resultData['documents'] as $variation)
            {
                $variationIdList[] = $variation['id'];
            }

            //Get the missing fields in ES from IDL
            if(is_array($variationIdList) && count($variationIdList) > 0)
            {
                /**
                 * @var \ElasticExport\ES_IDL_ResultList\BeezUp $idlResultList
                 */
                $idlResultList = pluginApp(\ElasticExport\ES_IDL_ResultList\BeezUp::class);
                $idlResultList = $idlResultList->getResultList($variationIdList, $settings);
            }

            //Creates an array with the variationId as key to surpass the sorting problem
            if(isset($idlResultList) && $idlResultList instanceof RecordList)
            {
                $this->createIdlArray($idlResultList);
            }

			foreach($resultData as $item)
			{
                $variationAttributes = $this->getVariationAttributes($item, $settings);

                $stockList = $this->getStockList($item);
                $price = $this->idlVariations[$item['id']];
                $rrp = $this->elasticExportHelper->getEsRecommendedRetailPrice($this->idlVariations[$item['id']]['variationRecommendedRetailPrice.price'], $settings);
                $rrp = ($price > $rrp) ? $price : $rrp;

                $shippingCost = $this->elasticExportHelper->getEsShippingCost($item['data']['item']['id'], $settings);

                if(!is_null($shippingCost))
                {
                    $shippingCost = number_format((float)$shippingCost, 2, ',', '');
                }
                else
                {
                    $shippingCost = '';
                }

				$data = [
					'Produkt ID'            =>  $item['data']['item']['id'],
                    'Artikel Nr'            =>  $this->idlVariations[$item['id']]['variationBase.customNumber'],
                    'MPN'                   =>  $item['data']['variation']['model'],
                    'EAN'                   =>  $this->elasticExportHelper->getEsBarcodeByType($item, $settings->get('barcode')),
                    'Marke'                 =>  $this->elasticExportHelper->getExternalManufacturerName((int)$item['data']['item']['manufacturer']['id']),
                    'Produktname'           =>  $this->elasticExportHelper->getEsName($item, $settings, 256),
                    'Produktbeschreibung'   =>  $this->getDescription($item, $settings),
                    'Preis inkl. MwSt.'     =>  number_format((float)$price, 2, '.', ''),
                    'UVP inkl. MwSt.'       =>  number_format((float)$rrp, 2, '.', ''),
                    'Produkt-URL'           =>  $this->elasticExportHelper->getEsUrl($item, $settings),
                    'Bild-URL'              =>  $this->getImageByNumber($item, $settings, 1),
                    'Bild-URL2'             =>  $this->getImageByNumber($item, $settings, 2),
                    'Bild-URL3'             =>  $this->getImageByNumber($item, $settings, 3),
                    'Bild-URL4'             =>  $this->getImageByNumber($item, $settings, 4),
                    'Bild-URL5'             =>  $this->getImageByNumber($item, $settings, 5),
                    'Lieferkosten'          =>  $shippingCost,
                    'Auf Lager'             =>  $stockList['variationAvailable'],
                    'Lagerbestand'          =>  $stockList['stock'],
                    'Lieferfrist'           =>  $this->elasticExportHelper->getEsAvailability($item['data']['defaultCategories'][0]['id'], $settings, false),
                    'Kategorie 1'           =>  $this->elasticExportHelper->getCategoryBranch($item['data']['defaultCategories'][0]['id'], $settings, 1),
                    'Kategorie 2'           =>  $this->elasticExportHelper->getCategoryBranch($item['data']['defaultCategories'][0]['id'], $settings, 2),
                    'Kategorie 3'           =>  $this->elasticExportHelper->getCategoryBranch($item['data']['defaultCategories'][0]['id'], $settings, 3),
                    'Kategorie 4'           =>  $this->elasticExportHelper->getCategoryBranch($item['data']['defaultCategories'][0]['id'], $settings, 4),
                    'Kategorie 5'           =>  $this->elasticExportHelper->getCategoryBranch($item['data']['defaultCategories'][0]['id'], $settings, 5),
                    'Kategorie 6'           =>  $this->elasticExportHelper->getCategoryBranch($item['data']['defaultCategories'][0]['id'], $settings, 6),
                    'Farbe'                 =>  $variationAttributes['Color'],
                    'Größe'                 =>  $variationAttributes['Size'],
                    'Gewicht'               =>  $item['data']['variation']['weightG'],
                    'Grundpreis'            =>  $this->elasticExportHelper->getEsBasePrice($item, $this->idlVariations[$item['id']]),
                    'ID'                    =>  $item['data']['item']['id'],
				];

				$this->addCSVContent(array_values($data));
			}
        }
    }

    /**
     * Get item description.
     * @param array $item
     * @param KeyValue $settings
     * @return string
     */
    private function getDescription($item, KeyValue $settings):string
    {
        $description = $this->elasticExportHelper->getEsItemCharacterByBackendName($item, $settings, self::PROPERTY_TYPE_DESCRIPTION);

        if (strlen($description) <= 0)
        {
            $description = $this->elasticExportHelper->getEsDescription($item, $settings, 5000);
        }

        return $description;
    }

    /**
     * Get variation attributes.
     * @param  array   $item
     * @param  KeyValue $settings
     * @return array<string,string>
     */
    private function getVariationAttributes($item, KeyValue $settings):array
    {
        $variationAttributes = [];

        foreach($item['data']['attributes'] as $variationAttribute)
        {
            $attributeValueName = $this->attributeValueNameRepository->findOne($variationAttribute['valueId'], $settings->get('lang'));

            if($attributeValueName instanceof AttributeValueName)
            {
                if($attributeValueName->attributeValue->attribute->amazonAttribute)
                {
                    $variationAttributes[$attributeValueName->attributeValue->attribute->amazonAttribute] = $attributeValueName->name;
                }
            }
        }

        return $variationAttributes;
    }

    /**
     * @param array $item
     * @param KeyValue $settings
     * @param int $number
     * @return string
     */
    private function getImageByNumber($item, KeyValue $settings, int $number):string
    {
        $imageList = $this->elasticExportHelper->getEsImageList($item, $settings);

        if(count($imageList) > 0 && array_key_exists($number, $imageList))
        {
            return $imageList[$number];
        }
        else
        {
            return '';
        }
    }

    /**
     * Get stock informations that depend on stock settings and stock volume
     * ($variationAvailable, $stock)
     * @param $item
     * @return array
     */
    private function getStockList($item):array
    {
        $variationAvailable = 'N';
        $stock = 0;

        if($item['data']['variation']['stockLimitation'] == 2)
        {
            $variationAvailable = 'Y';
            $stock = 999;
        }
        elseif($item['data']['variation']['stockLimitation'] == 1 && $this->idlVariations[$item['id']]['variationStock.stockNet'] > 0)
        {
            $variationAvailable = 'Y';
            if($this->idlVariations[$item['id']]['variationStock.stockNet'] > 999)
            {
                $stock = 999;
            }
            else
            {
                $stock = $this->idlVariations[$item['id']]['variationStock.stockNet'];
            }
        }
        elseif($item['data']['variation']['stockLimitation'] == 0)
        {
            $variationAvailable = 'Y';
            if($this->idlVariations[$item['id']]['variationStock.stockNet'] > 999)
            {
                $stock = 999;
            }
            else
            {
                if($this->idlVariations[$item['id']]['variationStock.stockNet'] > 0)
                {
                    $stock = $this->idlVariations[$item['id']]['variationStock.stockNet'];
                }
                else
                {
                    $stock = 0;
                }
            }
        }

        return array (
            'stock'                     =>  $stock,
            'variationAvailable'        =>  $variationAvailable,
        );

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
                        'itemBase.id'                           => $idlVariation->itemBase->id,
                        'variationBase.id'                      => $idlVariation->variationBase->id,
                        'variationBase.customNumber'            => $idlVariation->variationBase->customNumber,
                        'itemPropertyList'                      => $idlVariation->itemPropertyList,
                        'variationStock.stockNet'               => $idlVariation->variationStock->stockNet,
                        'variationRetailPrice.price'            => $idlVariation->variationRetailPrice->price,
                        'variationRecommendedRetailPrice.price' => $idlVariation->variationRecommendedRetailPrice->price,
                    ];
                }
            }
        }
    }
}

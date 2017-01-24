<?php

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Market\Helper\Contracts\MarketPropertyHelperRepositoryContract;

class RakutenDE extends CSVGenerator
{
	const PROPERTY_TYPE_ENERGY_CLASS       = 'energy_efficiency_class';
	const PROPERTY_TYPE_ENERGY_CLASS_GROUP = 'energy_efficiency_class_group';
	const PROPERTY_TYPE_ENERGY_CLASS_UNTIL = 'energy_efficiency_class_until';

	const MARKET_REFERENZ_RAKUTEN_DE = 106.00;

	/*
	 * @var ElasticExportHelper
	 */
	private $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

	/*
	 * @var array
	 */
	private $attributeName = array();

	/*
	 * @var array
	 */
	private $attributeNameCombination = array();

	/**
	 * MarketPropertyHelperRepositoryContract $marketPropertyHelperRepository
	 */
	private $marketPropertyHelperRepository;

	/**
	 * Rakuten constructor.
	 * @param ElasticExportHelper $elasticExportHelper
	 * @param ArrayHelper $arrayHelper
	 * @param MarketPropertyHelperRepositoryContract $marketPropertyHelperRepository
	 */
	public function __construct(
		ElasticExportHelper $elasticExportHelper,
		ArrayHelper $arrayHelper,
		MarketPropertyHelperRepositoryContract $marketPropertyHelperRepository
	)
	{
		$this->elasticExportHelper = $elasticExportHelper;
		$this->arrayHelper = $arrayHelper;
		$this->marketPropertyHelperRepository = $marketPropertyHelperRepository;
	}

	/**
	 * @param RecordList $resultData
	 * @param array $formatSettings
	 */
	protected function generateContent($resultData, array $formatSettings = [])
	{
		if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(";");

			$this->addCSVContent([
				'id',
				'variante_zu_id',
				'artikelnummer',
				'produkt_bestellbar',
				'produktname',
				'hersteller',
				'beschreibung',
				'variante',
				'variantenwert',
				'isbn_ean',
				'lagerbestand',
				'preis',
				'grundpreis_inhalt',
				'grundpreis_einheit',
				'reduzierter_preis',
				'bezug_reduzierter_preis',
				'mwst_klasse',
				'bestandsverwaltung_aktiv',
				'bild1',
				'bild2',
				'bild3',
				'bild4',
				'bild5',
				'kategorien',
				'lieferzeit',
				'tradoria_kategorie',
				'sichtbar',
				'free_var_1',
				'free_var_2',
				'free_var_3',
				'free_var_4',
				'free_var_5',
				'free_var_6',
				'free_var_7',
				'free_var_8',
				'free_var_9',
				'free_var_10',
				'free_var_11',
				'free_var_12',
				'free_var_13',
				'free_var_14',
				'free_var_15',
				'free_var_16',
				'free_var_17',
				'free_var_18',
				'free_var_19',
				'free_var_20',
				'MPN',
				'bild6',
				'bild7',
				'bild8',
				'bild9',
				'bild10',
				'technical_data',
				'energie_klassen_gruppe',
				'energie_klasse',
				'energie_klasse_bis',
				'energie_klassen_bild',
			]);

			$currentItemId = null;
			$previousItemId = null;
			$variations = array();
			foreach($resultData as $variation)
			{
				// Case first variation
				if ($currentItemId === null)
				{
					$previousItemId = $variation->itemBase->id;
				}
				$currentItemId = $variation->itemBase->id;

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
					$previousItemId = $variation->itemBase->id;
				}
			}

			// Write the las batch of variations
			if (is_array($variations) && count($variations) > 0)
			{
				$this->buildRows($settings, $variations);
			}
		}
	}

	/**
	 * @param $settings
	 * @param RecordList $variations
	 */
	private function buildRows($settings, $variations)
	{
		if (is_array($variations) && count($variations) > 0)
		{
			$primaryVariationKey = null;

			foreach($variations as $key => $variation)
			{
				/**
				 * Select and save the attribute name order for the first variation of each item with attributes,
				 * if the variation has attributes
				 */
				if (is_array($variation->variationAttributeValueList) &&
					count($variation->variationAttributeValueList) > 0 &&
					!array_key_exists($variation->itemBase->id, $this->attributeName) &&
					!array_key_exists($variation->itemBase->id, $this->attributeNameCombination))
				{
					$this->attributeName[$variation->itemBase->id] = $this->elasticExportHelper->getAttributeName($variation, $settings);

					foreach ($variation->variationAttributeValueList as $attribute)
					{
						$this->attributeNameCombination[$variation->itemBase->id][] = $attribute->attributeId;
					}
				}

				// note key of primary variation
				if($variation->variationBase->primaryVariation === true)
				{
					$primaryVariationKey = $key;
				}
			}

			// change sort of array and add primary variation as first entry
			if(!is_null($primaryVariationKey))
			{
				$primaryVariation = $variations[$primaryVariationKey];
				unset($variations[$primaryVariationKey]);
				array_unshift($variations, $primaryVariation);
			}

			$i = 1;
			foreach($variations as $key => $variation)
			{
				/**
				 * gets the attribute value name of each attribute value which is linked with the variation in a specific order,
				 * which depends on the $attributeNameCombination
				 */
				$attributeValue = $this->elasticExportHelper->getAttributeValueSetShortFrontendName($variation, $settings, '|', $this->attributeNameCombination[$variation->itemBase->id]);

				if(count($variations) == 1)
				{
					$this->buildParentWithoutChildrenRow($variation, $settings);
				}
				elseif($variation->variationBase->primaryVariation === false && $i == 1)
				{
					$this->buildParentWithChildrenRow($variation, $settings, $this->attributeName);
					$this->buildChildRow($variation, $settings, $attributeValue);
				}
				elseif($variation->variationBase->primaryVariation === true && strlen($attributeValue) > 0)
				{
					$this->buildParentWithChildrenRow($variation, $settings, $this->attributeName);
					$this->buildChildRow($variation, $settings, $attributeValue);
				}
				elseif($variation->variationBase->primaryVariation === true && strlen($attributeValue) == 0)
				{
					$this->buildParentWithChildrenRow($variation, $settings, $this->attributeName);
				}
				else
				{
					$this->buildChildRow($variation, $settings, $attributeValue);
				}

				$i++;
			}
		}
	}

	/**
	 * @param Record $item
	 * @param KeyValue $settings
	 * @return void
	 */
	private function buildParentWithoutChildrenRow(Record $item, KeyValue $settings)
	{

        $vat = $this->getVatClassId($item);

        $stockList = $this->getStockList($item);

        $priceList = $this->getPriceList($item, $settings);

        $basePriceComponentList = $this->getBasePriceComponentList($item);

		$data = [
			'id'						=> '',
			'variante_zu_id'			=> '',
			'artikelnummer'				=> $this->elasticExportHelper->generateSku($item, self::MARKET_REFERENZ_RAKUTEN_DE, (string)$item->variationMarketStatus->sku, (int)$settings->get('marketAccountId')),
			'produkt_bestellbar'		=> $stockList['variationAvailable'],
			'produktname'				=> $this->elasticExportHelper->getName($item, $settings, 150),
			'hersteller'				=> $this->elasticExportHelper->getExternalManufacturerName($item->itemBase->producerId),
			'beschreibung'				=> $this->elasticExportHelper->getDescription($item, $settings, 5000),
			'variante'					=> $this->elasticExportHelper->getAttributeName($item, $settings),
			'variantenwert'				=> '',
			'isbn_ean'					=> $this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode')),
			'lagerbestand'				=> $stockList['stock'],
            'preis'						=> number_format((float)$priceList['price'], 2, '.', ''),
            'grundpreis_inhalt'			=> strlen($basePriceComponentList['unit']) ?
                                                number_format((float)$basePriceComponentList['content'],3,',','') : '',
            'grundpreis_einheit'		=> $basePriceComponentList['unit'],
            'reduzierter_preis'			=> $priceList['reducedPrice'] > 0 ?
                                                number_format((float)$priceList['reducedPrice'], 2, '.', '') : '',
            'bezug_reduzierter_preis'	=> $priceList['referenceReducedPrice'],
			'mwst_klasse'				=> $vat,
			'bestandsverwaltung_aktiv'	=> $stockList['inventoryManagementActive'],
			'bild1'						=> $this->getImageByNumber($item, $settings, 0),
			'bild2'						=> $this->getImageByNumber($item, $settings, 1),
			'bild3'						=> $this->getImageByNumber($item, $settings, 2),
			'bild4'						=> $this->getImageByNumber($item, $settings, 3),
			'bild5'						=> $this->getImageByNumber($item, $settings, 4),
			'kategorien'				=> $this->elasticExportHelper->getCategory((int)$item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
			'lieferzeit'				=> $this->elasticExportHelper->getAvailability($item, $settings, false),
			'tradoria_kategorie'		=> $item->itemBase->tradoriaCategory,
			'sichtbar'					=> 1,
			'free_var_1'				=> $item->itemBase->free1,
			'free_var_2'				=> $item->itemBase->free2,
			'free_var_3'				=> $item->itemBase->free3,
			'free_var_4'				=> $item->itemBase->free4,
			'free_var_5'				=> $item->itemBase->free5,
			'free_var_6'				=> $item->itemBase->free6,
			'free_var_7'				=> $item->itemBase->free7,
			'free_var_8'				=> $item->itemBase->free8,
			'free_var_9'				=> $item->itemBase->free9,
			'free_var_10'				=> $item->itemBase->free10,
			'free_var_11'				=> $item->itemBase->free11,
			'free_var_12'				=> $item->itemBase->free12,
			'free_var_13'				=> $item->itemBase->free13,
			'free_var_14'				=> $item->itemBase->free14,
			'free_var_15'				=> $item->itemBase->free15,
			'free_var_16'				=> $item->itemBase->free16,
			'free_var_17'				=> $item->itemBase->free17,
			'free_var_18'				=> $item->itemBase->free18,
			'free_var_19'				=> $item->itemBase->free19,
			'free_var_20'				=> $item->itemBase->free20,
			'MPN'						=> $item->variationBase->model,
			'bild6'						=> $this->getImageByNumber($item, $settings, 5),
			'bild7'						=> $this->getImageByNumber($item, $settings, 6),
			'bild8'						=> $this->getImageByNumber($item, $settings, 7),
			'bild9'						=> $this->getImageByNumber($item, $settings, 8),
			'bild10'					=> $this->getImageByNumber($item, $settings, 9),
			'technical_data'			=> $this->elasticExportHelper->getTechnicalData($item, $settings),
			'energie_klassen_gruppe'	=> $this->getItemPropertyByExternalComponent($item, self::MARKET_REFERENZ_RAKUTEN_DE, self::PROPERTY_TYPE_ENERGY_CLASS_GROUP),
			'energie_klasse'			=> $this->getItemPropertyByExternalComponent($item, self::MARKET_REFERENZ_RAKUTEN_DE, self::PROPERTY_TYPE_ENERGY_CLASS),
			'energie_klasse_bis'		=> $this->getItemPropertyByExternalComponent($item, self::MARKET_REFERENZ_RAKUTEN_DE, self::PROPERTY_TYPE_ENERGY_CLASS_UNTIL),
			'energie_klassen_bild'		=> '',
		];

		$this->addCSVContent(array_values($data));
	}

	/**
	 * @param Record $item
	 * @param KeyValue $settings
     * @param array $attributeName
	 * @return void
	 */
	private function buildParentWithChildrenRow(Record $item, KeyValue $settings, array $attributeName)
	{
        $vat = $this->getVatClassId($item);

        $stockList = $this->getStockList($item);

		$data = [
			'id'						=> '#'.$item->itemBase->id,
			'variante_zu_id'			=> '',
			'artikelnummer'				=> '',
			'produkt_bestellbar'		=> '',
			'produktname'				=> $this->elasticExportHelper->getName($item, $settings, 150),
			'hersteller'				=> $this->elasticExportHelper->getExternalManufacturerName((int)$item->itemBase->producerId),
			'beschreibung'				=> $this->elasticExportHelper->getDescription($item, $settings, 5000),
			'variante'					=> $attributeName[$item->itemBase->id],
			'variantenwert'				=> '',
			'isbn_ean'					=> '',
			'lagerbestand'				=> '',
			'preis'						=> '',
			'grundpreis_inhalt'			=> '',
			'grundpreis_einheit'		=> '',
			'reduzierter_preis'			=> '',
			'bezug_reduzierter_preis'	=> '',
			'mwst_klasse'				=> $vat,
			'bestandsverwaltung_aktiv'	=> $stockList['inventoryManagementActive'],
			'bild1'						=> $this->getImageByNumber($item, $settings, 0),
			'bild2'						=> $this->getImageByNumber($item, $settings, 1),
			'bild3'						=> $this->getImageByNumber($item, $settings, 2),
			'bild4'						=> $this->getImageByNumber($item, $settings, 3),
			'bild5'						=> $this->getImageByNumber($item, $settings, 4),
			'kategorien'				=> $this->elasticExportHelper->getCategory((int)$item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
			'lieferzeit'				=> '',
			'tradoria_kategorie'		=> $item->itemBase->tradoriaCategory,
			'sichtbar'					=> 1,
			'free_var_1'				=> $item->itemBase->free1,
			'free_var_2'				=> $item->itemBase->free2,
			'free_var_3'				=> $item->itemBase->free3,
			'free_var_4'				=> $item->itemBase->free4,
			'free_var_5'				=> $item->itemBase->free5,
			'free_var_6'				=> $item->itemBase->free6,
			'free_var_7'				=> $item->itemBase->free7,
			'free_var_8'				=> $item->itemBase->free8,
			'free_var_9'				=> $item->itemBase->free9,
			'free_var_10'				=> $item->itemBase->free10,
			'free_var_11'				=> $item->itemBase->free11,
			'free_var_12'				=> $item->itemBase->free12,
			'free_var_13'				=> $item->itemBase->free13,
			'free_var_14'				=> $item->itemBase->free14,
			'free_var_15'				=> $item->itemBase->free15,
			'free_var_16'				=> $item->itemBase->free16,
			'free_var_17'				=> $item->itemBase->free17,
			'free_var_18'				=> $item->itemBase->free18,
			'free_var_19'				=> $item->itemBase->free19,
			'free_var_20'				=> $item->itemBase->free20,
			'MPN'						=> $item->variationBase->model,
			'bild6'						=> $this->getImageByNumber($item, $settings, 5),
			'bild7'						=> $this->getImageByNumber($item, $settings, 6),
			'bild8'						=> $this->getImageByNumber($item, $settings, 7),
			'bild9'						=> $this->getImageByNumber($item, $settings, 8),
			'bild10'					=> $this->getImageByNumber($item, $settings, 9),
			'technical_data'			=> $this->elasticExportHelper->getTechnicalData($item, $settings),
			'energie_klassen_gruppe'	=> '',
			'energie_klasse'			=> '',
			'energie_klasse_bis'		=> '',
			'energie_klassen_bild'		=> '',
		];

		$this->addCSVContent(array_values($data));
	}

	/**
	 * @param Record $item
	 * @param KeyValue $settings
     * @param string $attributeValue
	 * @return void
	 */
	private function buildChildRow(Record $item, KeyValue $settings, string $attributeValue = '')
	{

        $stockList = $this->getStockList($item);

        $priceList = $this->getPriceList($item, $settings);

        $basePriceComponentList = $this->getBasePriceComponentList($item);

		$data = [
			'id'						=> '',
			'variante_zu_id'			=> '#'.$item->itemBase->id,
			'artikelnummer'				=> $this->elasticExportHelper->generateSku($item, self::MARKET_REFERENZ_RAKUTEN_DE, (string)$item->variationMarketStatus->sku, (int)$settings->get('marketAccountId')),
			'produkt_bestellbar'		=> $stockList['variationAvailable'],
			'produktname'				=> '',
			'hersteller'				=> '',
			'beschreibung'				=> '',
			'variante'					=> '',
			'variantenwert'				=> $attributeValue,
			'isbn_ean'					=> $this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode')),
			'lagerbestand'				=> $stockList['stock'],
			'preis'						=> number_format((float)$priceList['price'], 2, '.', ''),
			'grundpreis_inhalt'			=> strlen($basePriceComponentList['unit']) ?
                                                number_format((float)$basePriceComponentList['content'],3,',','') : '',
			'grundpreis_einheit'		=> $basePriceComponentList['unit'],
			'reduzierter_preis'			=> $priceList['reducedPrice'] > 0 ?
                                                number_format((float)$priceList['reducedPrice'], 2, '.', '') : '',
			'bezug_reduzierter_preis'	=> $priceList['referenceReducedPrice'],
			'mwst_klasse'				=> '',
			'bestandsverwaltung_aktiv'	=> '',
			'bild1'						=> '',
			'bild2'						=> '',
			'bild3'						=> '',
			'bild4'						=> '',
			'bild5'						=> '',
			'kategorien'				=> '',
			'lieferzeit'				=> $this->elasticExportHelper->getAvailability($item, $settings, false),
			'tradoria_kategorie'		=> '',
			'sichtbar'					=> 1,
			'free_var_1'				=> $item->itemBase->free1,
			'free_var_2'				=> $item->itemBase->free2,
			'free_var_3'				=> $item->itemBase->free3,
			'free_var_4'				=> $item->itemBase->free4,
			'free_var_5'				=> $item->itemBase->free5,
			'free_var_6'				=> $item->itemBase->free6,
			'free_var_7'				=> $item->itemBase->free7,
			'free_var_8'				=> $item->itemBase->free8,
			'free_var_9'				=> $item->itemBase->free9,
			'free_var_10'				=> $item->itemBase->free10,
			'free_var_11'				=> $item->itemBase->free11,
			'free_var_12'				=> $item->itemBase->free12,
			'free_var_13'				=> $item->itemBase->free13,
			'free_var_14'				=> $item->itemBase->free14,
			'free_var_15'				=> $item->itemBase->free15,
			'free_var_16'				=> $item->itemBase->free16,
			'free_var_17'				=> $item->itemBase->free17,
			'free_var_18'				=> $item->itemBase->free18,
			'free_var_19'				=> $item->itemBase->free19,
			'free_var_20'				=> $item->itemBase->free20,
			'MPN'						=> $item->variationBase->model,
			'bild6'						=> '',
			'bild7'						=> '',
			'bild8'						=> '',
			'bild9'						=> '',
			'bild10'					=> '',
			'technical_data'			=> '',
			'energie_klassen_gruppe'	=> $this->getItemPropertyByExternalComponent($item, self::MARKET_REFERENZ_RAKUTEN_DE, self::PROPERTY_TYPE_ENERGY_CLASS_GROUP),
			'energie_klasse'			=> $this->getItemPropertyByExternalComponent($item, self::MARKET_REFERENZ_RAKUTEN_DE, self::PROPERTY_TYPE_ENERGY_CLASS),
			'energie_klasse_bis'		=> $this->getItemPropertyByExternalComponent($item, self::MARKET_REFERENZ_RAKUTEN_DE, self::PROPERTY_TYPE_ENERGY_CLASS_UNTIL),
			'energie_klassen_bild'		=> '',
		];

		$this->addCSVContent(array_values($data));
	}

	/**
	 * @param Record $item
	 * @param KeyValue $settings
	 * @param int $number
	 * @return string
	 */
	private function getImageByNumber(Record $item, KeyValue $settings, int $number):string
	{
		$imageList = $this->elasticExportHelper->getImageList($item, $settings);

		if(count($imageList) > 0 && array_key_exists($number, $imageList))
		{
			return (string)$imageList[$number];
		}
		else
		{
			return '';
		}
	}

	/**
	 * Returns the unit, if there is any unit configured, which is allowed
	 * for the Rakuten.de API.
	 *
	 * @param  Record   $item
	 * @return string
	 */
	private function getUnit(Record $item):string
	{
		switch((int) $item->variationBase->unitId)
		{
			case '32':
				return 'ml'; // Milliliter
			case '5':
				return 'l'; // Liter
			case '3':
				return 'g'; // Gramm
			case '2':
				return 'kg'; // Kilogramm
			case '51':
				return 'cm'; // Zentimeter
			case '31':
				return 'm'; // Meter
			case '38':
				return 'm²'; // Quadratmeter
			default:
				return '';
		}
	}

    /**
     * Get id for vat
     * @param Record $item
     * @return int
     */
	private function getVatClassId(Record $item):int
    {
        $vat = $item->variationRetailPrice->vatValue;
        if($vat == '10,7')
        {
            $vat = 4;
        }
        else if($vat == '7')
        {
            $vat = 2;
        }
        else if($vat == '0')
        {
            $vat = 3;
        }
        else
        {
            //bei anderen Steuersaetzen immer 19% nehmen
            $vat = 1;
        }
        return $vat;
    }

	/**
	 * Get item characters that match referrer from settings and a given component id.
	 * @param  Record   $item
	 * @param  float    $marketId
	 * @param  string  $externalComponent
	 * @return string
	 */
	private function getItemPropertyByExternalComponent(Record $item, float $marketId, $externalComponent):string
	{
		$marketProperties = $this->marketPropertyHelperRepository->getMarketProperty($marketId);

		foreach($item->itemPropertyList as $property)
		{
			foreach($marketProperties as $marketProperty)
			{
				if(is_array($marketProperty) && count($marketProperty) > 0 && $marketProperty['character_item_id'] == $property->propertyId)
				{
					if (strlen($externalComponent) > 0 && strpos($marketProperty['external_component'], $externalComponent) !== false)
					{
						$list = explode(':', $marketProperty['external_component']);
						if (isset($list[1]) && strlen($list[1]) > 0)
						{
							return $list[1];
						}
					}
				}
			}
		}

		return '';
	}

    /**
     * Get necessary components to enable Rakuten to calculate a base price for the variation
     * @param Record $item
     * @return array
     */
	private function getBasePriceComponentList(Record $item):array
    {
        $unit = $this->getUnit($item);
        $content = (float)$item->variationBase->content;
        $convertBasePriceContentTag = $this->elasticExportHelper->getConvertContentTag($content, 3);
        if ($convertBasePriceContentTag == true && strlen($unit))
        {
            $content = $this->elasticExportHelper->getConvertedBasePriceContent($content, $unit);
            $unit = $this->elasticExportHelper->getConvertedBasePriceUnit($unit);
        }
        return array(
            'content'   =>  $content,
            'unit'      =>  $unit,
        );
    }

    /**
     * Get all informations that depend on stock settings and stock volume
     * (inventoryManagementActive, $variationAvailable, $stock)
     * @param Record $item
     * @return array
     */
    private function getStockList(Record $item):array
    {
        $inventoryManagementActive = 0;
        $variationAvailable = 0;
        $stock = 0;

        if($item->variationBase->limitOrderByStockSelect == 2)
        {
            $variationAvailable = 1;
            $inventoryManagementActive = 0;
            $stock = 999;
        }
        elseif($item->variationBase->limitOrderByStockSelect == 1 && $item->variationStock->stockNet > 0)
        {
            $variationAvailable = 1;
            $inventoryManagementActive = 1;
            if($item->variationStock->stockNet > 999)
            {
                $stock = 999;
            }
            else
            {
                $stock = $item->variationStock->stockNet;
            }
        }
        elseif($item->variationBase->limitOrderByStockSelect == 0)
        {
            $variationAvailable = 1;
            $inventoryManagementActive = 0;
            if($item->variationStock->stockNet > 999)
            {
                $stock = 999;
            }
            else
            {
                if($item->variationStock->stockNet > 0)
                {
                    $stock = $item->variationStock->stockNet;
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
            'inventoryManagementActive' =>  $inventoryManagementActive,
        );

    }

	/**
     * Get a List of price, reduced price and the reference for the reduced price.
     * @param Record $item
     * @param KeyValue $settings
     * @return array
     */
    private function getPriceList(Record $item, KeyValue $settings):array
    {
        $variationPrice = $this->elasticExportHelper->getPrice($item);
        $variationRrp = $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings);
        $variationSpecialPrice = $this->elasticExportHelper->getSpecialPrice($item, $settings);

        //setting retail price as selling price without a reduced price
        $price = $variationPrice;
        $reducedPrice = '';
        $referenceReducedPrice = '';

        if ($price != '' || $price != 0.00)
        {
            //if recommended retail price is set and higher than retail price...
            if ($variationRrp > 0 && $variationRrp > $variationPrice)
            {
                //set recommended retail price as selling price
                $price = $variationRrp;
                //set retail price as reduced price
                $reducedPrice = $variationPrice;
                //set recommended retail price as reference
                $referenceReducedPrice = 'UVP';
            }

            // if special offer price is set and lower than retail price and recommended retail price is already set as reference...
            if ($variationSpecialPrice > 0 && $variationPrice > $variationSpecialPrice && $referenceReducedPrice == 'UVP')
            {
                //set special offer price as reduced price
                $reducedPrice = $variationSpecialPrice;
            }
            //if recommended retail price is not set as reference then ...
            elseif ($variationSpecialPrice > 0 && $variationPrice > $variationSpecialPrice)
            {
                //set special offer price as reduced price and...
                $reducedPrice = $variationSpecialPrice;
                //set retail price as reference
                $referenceReducedPrice = 'VK';
            }
        }
        return array(
            'price'                     =>  $price,
            'reducedPrice'              =>  $reducedPrice,
            'referenceReducedPrice'     =>  $referenceReducedPrice
        );
    }
}

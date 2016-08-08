<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;

class RakutenDE extends CSVGenerator
{
	const string CHARACTER_TYPE_ENERGY_CLASS		= 'energie_klasse';
	const string CHARACTER_TYPE_ENERGY_CLASS_GROUP	= 'energie_klassen_gruppe';
	const string CHARACTER_TYPE_ENERGY_CLASS_UNTIL	= 'energie_klasse_bis';

	/*
	 * @var ElasticExportHelper
	 */
	private ElasticExportHelper $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
	private ArrayHelper $arrayHelper;

	/**
	 * Rakuten constructor.
	 * @param ElasticExportHelper $elasticExportHelper
	 * @param ArrayHelper $arrayHelper
	 */
	public function __construct(ElasticExportHelper $elasticExportHelper, ArrayHelper $arrayHelper)
	{
		$this->elasticExportHelper = $elasticExportHelper;
		$this->arrayHelper = $arrayHelper;
	}

	/**
	 * @param mixed $resultData
	 */
	protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
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
				'MPN'		 ,
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

			$previousItemId = 0;
            $attributeName = array();
            foreach($resultData as $item)
            {
                $attributeName[$item->itemBase->id] = $this->elasticExportHelper->getAttributeName($item, $settings);
            }

			foreach($resultData as $item)
			{
				$currentItemId = $item->itemBase->id;
                $attributeValue = $this->elasticExportHelper->getAttributeValueSetShortFrontendName($item, $settings, '|');

                /**
                 * Case of an item with more variation
                 */
                if ($previousItemId != $currentItemId && $item->itemBase->variationCount > 1)
				{
                    /**
                     * The item has multiple active variations with attributes
                     */
                    if(strlen($attributeName[$item->itemBase->id]) > 0)
                    {
                        $this->buildParentWithChildrenRow($item, $settings, $attributeName);
                    }
                    /**
                     * The item has only inactive variations
                     */
                    else
                    {
                        $this->buildParentWithoutChildrenRow($item, $settings);
                    }
                    /**
                     * This will only be triggered if the main variation also has a attribute value
                     */
					if(strlen($attributeValue) > 0)
					{
						$this->buildChildRow($item, $settings, $attributeValue);
					}
					$previousItemId = $currentItemId;
				}
                /**
                 * Case item has only the main variation
                 */
				elseif($previousItemId != $currentItemId && $item->itemBase->variationCount == 1 && $item->itemBase->hasAttribute == false)
				{
					$this->buildParentWithoutChildrenRow($item, $settings);
					$previousItemId = $currentItemId;
				}
                /**
                 * The parent is already in the csv
                 */
				elseif(strlen($attributeValue) > 0)
				{
					$this->buildChildRow($item, $settings, $attributeValue);
				}
			}
		}
	}

	/**
	 * @param Record $item
	 * @param KeyValue $settings
	 * @return void
	 */
	private function buildParentWithoutChildrenRow(Record $item, KeyValue $settings):void
	{
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
		else
		{
			$variationAvailable = 0;
			$inventoryManagementActive = 1;
			$stock = 0;
		}

		$vat = $item->variationBase->vatId;
		if($vat == '19')
		{
			$vat = 1;
		}
		else if($vat == '10,7')
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
			//bei anderen Steuersätzen immer 19% nehmen
			$vat = 1;
		}

		$rrp = $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) > $this->elasticExportHelper->getPrice($item) ? $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) : $this->elasticExportHelper->getPrice($item);
		$price = $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) > $this->elasticExportHelper->getPrice($item) ? $this->elasticExportHelper->getPrice($item) : $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings);
		$price = $price > 0 ? $price : '';
		$unit = $this->getUnit($item, $settings);
		$basePriceContent = (int)$item->variationBase->content;

		$data = [
			'id'						=> '',
			'variante_zu_id'			=> '',
			'artikelnummer'				=> $this->elasticExportHelper->generateSku($item, 106, $item->variationMarketStatus->sku),
			'produkt_bestellbar'		=> $variationAvailable,
			'produktname'				=> $this->elasticExportHelper->getName($item, $settings, 150),
			'hersteller'				=> $item->itemBase->producer,
			'beschreibung'				=> $this->elasticExportHelper->getDescription($item, $settings, 5000),
			'variante'					=> $this->elasticExportHelper->getAttributeName($item, $settings),
			'variantenwert'				=> '',
			'isbn_ean'					=> $this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode')),
			'lagerbestand'				=> $stock,
			'preis'						=> number_format($rrp, 2, '.', ''),
			'grundpreis_inhalt'			=> strlen($unit) > 0 ? $basePriceContent : '',
			'grundpreis_einheit'		=> $unit,
			'reduzierter_preis'			=> number_format($price, 2, '.', ''),
			'bezug_reduzierter_preis'	=> 'UVP',
			'mwst_klasse'				=> $vat,
			'bestandsverwaltung_aktiv'	=> $inventoryManagementActive,
			'bild1'						=> $this->getImageByNumber($item, $settings, 0),
			'bild2'						=> $this->getImageByNumber($item, $settings, 1),
			'bild3'						=> $this->getImageByNumber($item, $settings, 2),
			'bild4'						=> $this->getImageByNumber($item, $settings, 3),
			'bild5'						=> $this->getImageByNumber($item, $settings, 4),
			'kategorien'				=> $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),	//todo kategorie name
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
			'technical_data'			=> $item->itemDescription->technicalData,
			'energie_klassen_gruppe'	=> $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, self::CHARACTER_TYPE_ENERGY_CLASS_GROUP),
			'energie_klasse'			=> $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, self::CHARACTER_TYPE_ENERGY_CLASS),
			'energie_klasse_bis'		=> $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, self::CHARACTER_TYPE_ENERGY_CLASS_UNTIL),
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
	private function buildParentWithChildrenRow(Record $item, KeyValue $settings, array<int, mixed> $attributeName):void
	{
        $vat = $item->variationBase->vatId;
        if($vat == '19')
        {
            $vat = 1;
        }
        else if($vat == '10,7')
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

        if($item->variationBase->limitOrderByStockSelect == 2)
        {
            $inventoryManagementActive = 0;
        }
        elseif($item->variationBase->limitOrderByStockSelect == 1 && $item->variationStock->stockNet > 0)
        {
            $inventoryManagementActive = 1;
        }
        elseif($item->variationBase->limitOrderByStockSelect == 0)
        {
            $inventoryManagementActive = 0;
        }
        else
        {
            $inventoryManagementActive = 1;
        }

		$data = [
			'id'						=> '#'.$item->itemBase->id,
			'variante_zu_id'			=> '',
			'artikelnummer'				=> '',
			'produkt_bestellbar'		=> '',
			'produktname'				=> $this->elasticExportHelper->getName($item, $settings, 150),
			'hersteller'				=> '',
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
			'bestandsverwaltung_aktiv'	=> $inventoryManagementActive,
			'bild1'						=> $this->getImageByNumber($item, $settings, 0),
			'bild2'						=> $this->getImageByNumber($item, $settings, 1),
			'bild3'						=> $this->getImageByNumber($item, $settings, 2),
			'bild4'						=> $this->getImageByNumber($item, $settings, 3),
			'bild5'						=> $this->getImageByNumber($item, $settings, 4),
			'kategorien'				=> $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
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
			'technical_data'			=> $item->itemDescription->technicalData,
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
	private function buildChildRow(Record $item, KeyValue $settings, string $attributeValue = ''):void
	{
		if($item->variationBase->limitOrderByStockSelect == 2)
		{
			$variationAvailable = 1;
			$stock = 999;
		}
		elseif($item->variationBase->limitOrderByStockSelect == 1 && $item->variationStock->stockNet > 0)
		{
			$variationAvailable = 1;
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
		else
		{
			$variationAvailable = 0;
			$stock = 0;
		}


		$rrp = $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) > $this->elasticExportHelper->getPrice($item) ? $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) : $this->elasticExportHelper->getPrice($item);
		$price = $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) > $this->elasticExportHelper->getPrice($item) ? $this->elasticExportHelper->getPrice($item) : $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings);
		$price = $price > 0 ? $price : '';

		$unit = $this->getUnit($item, $settings);
		$basePriceContent = (int)$item->variationBase->content;

		$data = [
			'id'						=> '',
			'variante_zu_id'			=> '#'.$item->itemBase->id,
			'artikelnummer'				=> $this->elasticExportHelper->generateSku($item, 106, $item->variationMarketStatus->sku),
			'produkt_bestellbar'		=> $variationAvailable,
			'produktname'				=> '',
			'hersteller'				=> $item->itemBase->producer,
			'beschreibung'				=> '',
			'variante'					=> '',
			'variantenwert'				=> $attributeValue,
			'isbn_ean'					=> $this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode')),
			'lagerbestand'				=> $stock,
			'preis'						=> number_format($rrp, 2, '.', ''),
			'grundpreis_inhalt'			=> strlen($unit) ? $basePriceContent : '',
			'grundpreis_einheit'		=> $unit,
			'reduzierter_preis'			=> number_format($price, 2, '.', ''),
			'bezug_reduzierter_preis'	=> 'UVP',
			'mwst_klasse'				=> '',
			'bestandsverwaltung_aktiv'	=> '',
			'bild1'						=> $this->getImageByNumber($item, $settings, 0),
			'bild2'						=> $this->getImageByNumber($item, $settings, 1),
			'bild3'						=> $this->getImageByNumber($item, $settings, 2),
			'bild4'						=> $this->getImageByNumber($item, $settings, 3),
			'bild5'						=> $this->getImageByNumber($item, $settings, 4),
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
			'bild6'						=> $this->getImageByNumber($item, $settings, 5),
			'bild7'						=> $this->getImageByNumber($item, $settings, 6),
			'bild8'						=> $this->getImageByNumber($item, $settings, 7),
			'bild9'						=> $this->getImageByNumber($item, $settings, 8),
			'bild10'					=> $this->getImageByNumber($item, $settings, 9),
			'technical_data'			=> '',
			'energie_klassen_gruppe'	=> $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, self::CHARACTER_TYPE_ENERGY_CLASS_GROUP),
			'energie_klasse'			=> $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, self::CHARACTER_TYPE_ENERGY_CLASS),
			'energie_klasse_bis'		=> $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, self::CHARACTER_TYPE_ENERGY_CLASS_UNTIL),
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
			return $imageList[$number];
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
	 * @param  KeyValue $settings
	 * @return string
	 */
	private function getUnit(Record $item, KeyValue $settings):string
	{
		$unit = $this->elasticExportHelper->getBasePriceDetailUnit($item, $settings);

		switch($unit)
		{
			case 'MLT':
				return 'ml'; // Milliliter
			case 'LTR':
				return 'l'; // Liter
			case 'GRM':
				return 'g'; // Gramm
			case 'KGM':
				return 'kg'; // Kilogramm
			case 'CTM':
				return 'cm'; // Zentimeter
			case 'MTR':
				return 'm'; // Meter
			case 'MTK':
				return 'm²'; // Quadratmeter
			default:
				return '';
		}
	}
}

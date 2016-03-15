<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Unit\Models\UnitLang;
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

			foreach($resultData as $item)
			{
				$currentItemId = $item->itemBase->id;
				if ($previousItemId != $currentItemId)
				{
					$this->buildParentRow($item, $settings);
					$this->buildChildRow($item, $settings);
					$previousItemId = $currentItemId;
				}
				else
				{
					$this->buildChildRow($item, $settings);
				}
			}
		}
	}

	/**
	 * @param Record $item
	 * @param KeyValue $settings
	 * @return void
	 */
	private function buildParentRow(Record $item, KeyValue $settings):void
	{
		$data = [
			'id'						=> '#'.$item->itemBase->id,
			'variante_zu_id'			=> '',
			'artikelnummer'				=> '',
			'produkt_bestellbar'		=> '',
			'produktname'				=> '',
			'hersteller'				=> $item->itemBase->producer,
			'beschreibung'				=> $this->elasticExportHelper->getDescription($item, $settings, 5000),
			'variante'					=> '',
			'variantenwert'				=> '',
			'isbn_ean'					=> '',
			'lagerbestand'				=> '',
			'preis'						=> '',
			'grundpreis_inhalt'			=> '',
			'grundpreis_einheit'		=> '',
			'reduzierter_preis'			=> '',
			'bezug_reduzierter_preis'	=> '',
			'mwst_klasse'				=> '',
			'bestandsverwaltung_aktiv'	=> '',
			'bild1'						=> $this->getImageByNumber($item, $settings, 1),
			'bild2'						=> $this->getImageByNumber($item, $settings, 2),
			'bild3'						=> $this->getImageByNumber($item, $settings, 3),
			'bild4'						=> $this->getImageByNumber($item, $settings, 4),
			'bild5'						=> $this->getImageByNumber($item, $settings, 5),
			'kategorien'				=> '',
			'lieferzeit'				=> '',
			'tradoria_kategorie'		=> '',
			'sichtbar'					=> '',
			'free_var_1'				=> '',
			'free_var_2'				=> '',
			'free_var_3'				=> '',
			'free_var_4'				=> '',
			'free_var_5'				=> '',
			'free_var_6'				=> '',
			'free_var_7'				=> '',
			'free_var_8'				=> '',
			'free_var_9'				=> '',
			'free_var_10'				=> '',
			'free_var_11'				=> '',
			'free_var_12'				=> '',
			'free_var_13'				=> '',
			'free_var_14'				=> '',
			'free_var_15'				=> '',
			'free_var_16'				=> '',
			'free_var_17'				=> '',
			'free_var_18'				=> '',
			'free_var_19'				=> '',
			'free_var_20'				=> '',
			'MPN'						=> '',
			'bild6'						=> $this->getImageByNumber($item, $settings, 6),
			'bild7'						=> $this->getImageByNumber($item, $settings, 7),
			'bild8'						=> $this->getImageByNumber($item, $settings, 8),
			'bild9'						=> $this->getImageByNumber($item, $settings, 9),
			'bild10'					=> $this->getImageByNumber($item, $settings, 10),
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
	 * @return void
	 */
	private function buildChildRow(Record $item, KeyValue $settings):void
	{
		$data = [
			'id'						=> $item->itemBase->id,
			'variante_zu_id'			=> $item->variationBase->id,
			'artikelnummer'				=> $item->variationBase->customNumber,
			'produkt_bestellbar'		=> '',
			'produktname'				=> $this->elasticExportHelper->getName($item, $settings, 150),
			'hersteller'				=> $item->itemBase->producer,
			'beschreibung'				=> '',
			'variante'					=> '',
			'variantenwert'				=> '',
			'isbn_ean'					=> $item->variationBarcode->code,
			'lagerbestand'				=> '',
			'preis'						=> number_format($this->elasticExportHelper->getPrice($item, $settings), 2, '.', ''),
			'grundpreis_inhalt'			=> '',
			'grundpreis_einheit'		=> '',
			'reduzierter_preis'			=> '',
			'bezug_reduzierter_preis'	=> '',
			'mwst_klasse'				=> $item->variationBase->vatId,
			'bestandsverwaltung_aktiv'	=> '',
			'bild1'						=> $this->getImageByNumber($item, $settings, 1),
			'bild2'						=> $this->getImageByNumber($item, $settings, 2),
			'bild3'						=> $this->getImageByNumber($item, $settings, 3),
			'bild4'						=> $this->getImageByNumber($item, $settings, 4),
			'bild5'						=> $this->getImageByNumber($item, $settings, 5),
			'kategorien'				=> $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
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
			'bild6'						=> $this->getImageByNumber($item, $settings, 6),
			'bild7'						=> $this->getImageByNumber($item, $settings, 7),
			'bild8'						=> $this->getImageByNumber($item, $settings, 8),
			'bild9'						=> $this->getImageByNumber($item, $settings, 9),
			'bild10'					=> $this->getImageByNumber($item, $settings, 10),
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
}

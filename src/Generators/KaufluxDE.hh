<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Character\Contracts\CharacterSelectionRepositoryContract;
use Plenty\Modules\Character\Models\CharacterSelection;

/**
 * Class KaufluxDE
 * @package ElasticExport\Generators
 */
class KaufluxDE extends CSVGenerator
{
	const int STATUS_VISIBLE = 0;
	const int STATUS_LOCKED = 1;
	const int STATUS_HIDDEN = 2;

	/*
     * @var
     */
    private ElasticExportHelper $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
	private ArrayHelper $arrayHelper;

	/**
	 * CharacterSelectionRepositoryContract $characterSelectionRepository
	 */
	private CharacterSelectionRepositoryContract $characterSelectionRepository;

	/**
	 * @var array<int,mixed>
	 */
	private array<int,array<string>>$itemPropertyCache = [];

	/**
	 * @var array<int>
	 */
	private array<int> $addedItems = [];

	/**
	 * @var ImmMap<int,string>
	 */
	private ImmMap<int,string> $flags = ImmMap{
		0 => '',
		1 => 'Sonderangebot',
		2 => 'Neuheit',
		3 => 'Top Artikel',
	};

    /**
     * IdealoGenerator constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(
		ElasticExportHelper $elasticExportHelper,
		ArrayHelper $arrayHelper,
		CharacterSelectionRepositoryContract $characterSelectionRepository

	)
    {
        $this->elasticExportHelper = $elasticExportHelper;
		$this->arrayHelper = $arrayHelper;
		$this->characterSelectionRepository = $characterSelectionRepository;
    }

    protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
    {
        if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(";");

			$this->addCSVContent([
				'GroupID',
				'BestellNr',
				'EAN',
				'Hersteller',
				'BestandModus',
				'BestandAbsolut',
				'Liefertyp',
				'VersandKlasse',
				'Lieferzeit',
				'Umtausch',
				'Bezeichnung',
				'KurzText',
				'DetailText',
				'Keywords',
				'Bild1',
				'Bild2',
				'Bild3',
				'Gewicht',
				'Preis',
				'MwSt',
				'UVP',
				'Katalog1',
				'Flags',
				'LinkXS',
				'ExtLinkDetail',
				'Status',
				'FreeVar1',
				'FreeVar2',
				'FreeVar3',
				'InhaltMenge',
				'InhaltEinheit',
				'InhaltVergleich',
				'HerstellerArtNr',
			]);

			foreach($resultData as $item)
			{
				$data = [
					'GroupID' 			=> $item->itemBase->id,
					'BestellNr' 		=> $item->variationMarketStatus->sku,
					'EAN' 				=> $item->variationBarcode->code,
					'Hersteller' 		=> $item->itemBase->producer,
					'BestandModus' 		=> '', // TODO get from config
					'BestandAbsolut' 	=> '', // TODO get from config
					'Liefertyp' 		=> 'V',
					'VersandKlasse' 	=> '', // TODO get from config
					'Lieferzeit' 		=> (int) $this->elasticExportHelper->getAvailability($item, $settings, false),
					'Umtausch' 			=> '', // TODO get from config
					'Bezeichnung' 		=> $this->elasticExportHelper->getName($item, $settings) . ' ' . $item->variationBase->variationName,
					'KurzText' 			=> $this->elasticExportHelper->getPreviewText($item, $settings),
					'DetailText' 		=> $this->elasticExportHelper->getDescription($item, $settings) . ' ' . $this->getPropertyDescription($item, $settings),
					'Keywords' 			=> $item->itemDescription->keywords,
					'Bild1' 			=> $this->getImageByNumber($item, $settings, 1),
					'Bild2' 			=> $this->getImageByNumber($item, $settings, 2),
					'Bild3' 			=> $this->getImageByNumber($item, $settings, 3),
					'Gewicht' 			=> $item->variationBase->weightG,
					'Preis' 			=> number_format($this->elasticExportHelper->getPrice($item, $settings), 2, '.', ''),
					'MwSt' 				=> $item->variationRetailPrice->vatValue,
					'UVP' 				=> $item->variationRecommendedRetailPrice->price,
					'Katalog1' 			=> $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('lang'), $settings->get('plentyId')),
					'Flags' 			=> $this->flags->get($item->itemBase->storeSpecial),
					'LinkXS' 			=> implode(', ', $this->getCrossSellingItems($item)),
					'ExtLinkDetail' 	=> $this->elasticExportHelper->getUrl($item, $settings),
					'Status' 			=> $this->getStatus($item),
					'FreeVar1' 			=> $item->itemBase->free1,
					'FreeVar2' 			=> $item->itemBase->free2,
					'FreeVar3' 			=> $item->itemBase->free3,
					'InhaltMenge' 		=> '',
					'InhaltEinheit' 	=> '',
					'InhaltVergleich' 	=> '',
					'HerstellerArtNr' 	=> $item->variationBase->model,
				];

				$this->addCSVContent(array_values($data));
			}
		}
    }

	/**
	 * Get description of all correlated properties
	 * @param Record $item
	 * @param KeyValue $settings
	 * @return string
	 */
	private function getPropertyDescription(Record $item, KeyValue $settings):string
	{
		$properties = $this->getItemPropertyList($item, $settings);

		$propertyDescription = '';

		foreach($properties as $property)
		{
			$propertyDescription .= '<br/>' . $property;
		}

		return $propertyDescription;
	}

	/**
	 * Get item properties.
	 * @param 	Record $item
	 * @param  KeyValue $settings
	 * @return array<string,string>
	 */
	private function getItemPropertyList(Record $item, KeyValue $settings):array<string>
	{
		if(!array_key_exists($item->itemBase->id, $this->itemPropertyCache))
		{
			$characterMarketComponentList = $this->elasticExportHelper->getItemCharactersByComponent($item, $settings, 1);

			$list = [];

			if(count($characterMarketComponentList))
			{
				foreach($characterMarketComponentList as $data)
				{
					if((string) $data['characterValueType'] != 'file' && (string) $data['characterValueType'] != 'empty')
					{
						if((string) $data['characterValueType'] == 'selection')
						{
							$characterSelection = $this->characterSelectionRepository->findCharacterSelection((int) $data['characterValue']);
							if($characterSelection instanceof CharacterSelection)
							{
								$list[] = (string) $characterSelection->name;
							}
						}
						else
						{
							$list[] = (string) $data['characterValue'];
						}

					}
				}
			}

			$this->itemPropertyCache[$item->itemBase->id] = $list;
		}

		return $this->itemPropertyCache[$item->itemBase->id];
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
	 * Get list of cross selling items.
	 * @param Record $item
	 * @return array<string>
	 */
	private function getCrossSellingItems(Record $item):array<string>
	{
		$list = [];

		foreach($item->itemCrossSellingList as $itemCrossSelling)
		{
			$list[] = (string) $itemCrossSelling->crossItemId;
		}

		return $list;
	}

	/**
	 * Get status.
	 * @param  Record $item
	 * @return int
	 */
	private function getStatus(Record $item):int
	{
		if(!array_key_exists($item->itemBase->id, $this->addedItems))
		{
			$this->addedItems[$item->itemBase->id] = $item->itemBase->id;

			return self::STATUS_VISIBLE;
		}

		return self::STATUS_HIDDEN;
	}
}

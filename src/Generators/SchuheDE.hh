<?hh // strict

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Item\Attribute\Contracts\AttributeValueLangRepositoryContract;
use Plenty\Modules\Item\Attribute\Models\AttributeValueLang;
use Plenty\Modules\Character\Contracts\CharacterSelectionRepositoryContract;
use Plenty\Modules\Character\Models\CharacterSelection;

class SchuheDE extends CSVGenerator
{
	/*
     * @var ElasticExportHelper
     */
    private ElasticExportHelper $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
	private ArrayHelper $arrayHelper;

	/**
	 * AttributeValueLangRepositoryContract $attributeValueLangRepository
	 */
	private AttributeValueLangRepositoryContract $attributeValueLangRepository;

	/**
	 * CharacterSelectionRepositoryContract $characterSelectionRepository
	 */
	private CharacterSelectionRepositoryContract $characterSelectionRepository;

	/**
	 * @var array<int,mixed>
	 */
	private array<int,array<string,string>>$itemPropertyCache = [];

	/**
     * Geizhals constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     * @param AttributeValueLangRepositoryContract $attributeValueLangRepository
     * @param CharacterSelectionRepositoryContract $characterSelectionRepository
     */
    public function __construct(
    	ElasticExportHelper $elasticExportHelper, 
    	ArrayHelper $arrayHelper, 
    	AttributeValueLangRepositoryContract $attributeValueLangRepository,
    	CharacterSelectionRepositoryContract $characterSelectionRepository
    )
    {
		$this->elasticExportHelper = $elasticExportHelper;
		$this->arrayHelper = $arrayHelper;
		$this->attributeValueLangRepository = $attributeValueLangRepository;
		$this->characterSelectionRepository = $characterSelectionRepository;
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
				'Identnummer',					
				'Artikelnummer',				
				'Herstellerartikelnummer',		
				'Artikelname',					
				'Artikelbeschreibung',
				'Bild' . '(er)',						
				'360 Grad',
				'Bestand',						
				'Farbe',						
				'Farbe Suche I',
				'Farbe Suche II',
				'Hersteller Farbbezeichnung',
				'GG Größengang',				
				'Größe',						
				'Marke',						
				'Saison',						
				'EAN',
				'Währung',						
				'Versandkosten',				
				'Info Versandkosten',
				'Preis' . ' (UVP)',					
				'reduzierter Preis',
				'Grundpreis',
				'Grundpreis Einheit',
				'Kategorien',					
				'Link',							
				'Anzahl Verkäufe',
				'Schuhbreite',
				'Absatzhöhe',
				'Absatzform',
				'Schuhspitze',
				'Obermaterial',
				'Schaftweite',
				'Schafthöhe',
				'Materialzusammensetzung',
				'Besonderheiten',
				'Verschluss',
				'Innenmaterial',
				'Sohle',
				'Größenhinweis',
				'Wechselfussbett',
				'Wasserdicht',
				'Promotion',
				'URL Video',
				'Steuersatz',
				'ANWR schuh Trend',
			]);

			$rows = [];

			foreach($resultData as $item)
			{
				$itemName = strlen($this->elasticExportHelper->getName($item, $settings, 256)) <= 0 ? $item->variationBase->id : $this->elasticExportHelper->getName($item, $settings, 256);

				$rrp = $item->variationRecommendedRetailPrice->price > $this->elasticExportHelper->getPrice($item, $settings) ? $item->variationRecommendedRetailPrice->price : $this->elasticExportHelper->getPrice($item, $settings);
				$price = $item->variationRecommendedRetailPrice->price > $this->elasticExportHelper->getPrice($item, $settings) ? $this->elasticExportHelper->getPrice($item, $settings) : $item->variationRecommendedRetailPrice->price;

				$data = [
					'Identnummer'					=> $item->variationBase->id,					
					'Artikelnummer'					=> $item->variationBase->customNumber,
					'Herstellerartikelnummer'		=> $item->variationBase->model,		
					'Artikelname'					=> $itemName,					
					'Artikelbeschreibung'			=> $this->elasticExportHelper->getDescription($item, $settings, 256),
					'Bild' . '(er)'						=> $this->elasticExportHelper->getImageList($item, $settings, ';'),
					'360 Grad'						=> '',
					'Bestand'						=> $this->getStock($item, $settings),						
					'Farbe'							=> '',
					'Farbe Suche I'					=> '',
					'Farbe Suche II'				=> '',
					'Hersteller Farbbezeichnung'	=> '',
					'GG Größengang'					=> '',
					'Größe'							=> '',
					'Marke'							=> $item->itemBase->producer,
					'Saison'						=> '',						
					'EAN'							=> $item->variationBarcode->code,
					'Währung'						=> $settings->get('currency'),
					'Versandkosten'					=> number_format($this->elasticExportHelper->getShippingCost($item, $settings), 2, '.', ''),				
					'Info Versandkosten'			=> '',
					'Preis' . ' (UVP)'				=> number_format($rrp, 2, '.', ''),					
					'reduzierter Preis'				=> number_format($price, 2, '.', ''),
					'Grundpreis'					=> '', // TODO
					'Grundpreis Einheit'			=> '', // TODO
					'Kategorien'					=> '', // TODO
					'Link'							=> $this->elasticExportHelper->getUrl($item, $settings),							
					'Anzahl Verkäufe'				=> '',
					'Schuhbreite'					=> '',
					'Absatzhöhe'					=> '',
					'Absatzform'					=> '',
					'Schuhspitze'					=> '',
					'Obermaterial'					=> '',
					'Schaftweite'					=> '',
					'Schafthöhe'					=> '',
					'Materialzusammensetzung' 		=> '',
					'Besonderheiten'				=> '',
					'Verschluss'					=> '',
					'Innenmaterial'					=> '',
					'Sohle'							=> '',
					'Größenhinweis'					=> '',
					'Wechselfussbett'				=> '',
					'Wasserdicht'					=> '',
					'Promotion'						=> '',
					'URL Video'						=> '',
					'Steuersatz'					=> '',
					'ANWR schuh Trend'				=> '',
				];

				$this->addCSVContent(array_values($data));
			}			
		}
	}


	/**
	 * Get item properties. 
	 * @param 	Record $item
	 * @param  KeyValue $settings
	 * @return array<string,string>
	 */
	protected function getItemPropertyList(Record $item, KeyValue $settings):array<string,string>
	{
		if(!array_key_exists($item->itemBase->id, $this->itemPropertyCache))
		{
			$characterMarketComponentList = $this->elasticExportHelper->getItemCharactersByComponent($item, $settings);

			$list = [];

			if(count($characterMarketComponentList))
			{
				foreach($characterMarketComponentList as $data)
				{
					if((string) $data['characterValueType'] != 'file' && (string) $data['characterValueType'] != 'empty' && (string) $data['externalComponent'] != "0")
					{
						if((string) $data['characterValueType'] == 'selection')
						{
							$characterSelection = $this->characterSelectionRepository->findCharacterSelection((int) $data['characterValue']);
							if($characterSelection instanceof CharacterSelection)
							{
								$list[(string) $data['externalComponent']] = (string) $characterSelection->name;			
							}
						}
						else
						{
							$list[(string) $data['externalComponent']] = (string) $data['characterValue'];	
						}
						
					}					
				}
			}	

			$this->itemPropertyCache[$item->itemBase->id] = $list;
		}
		
		return $this->itemPropertyCache[$item->itemBase->id];
	}

	/**
	 * Get available stock.
	 * @param  Record $item
	 * @param  KeyValue $settings
	 * @return int
	 */
	private function getStock(Record $item, KeyValue $settings):int
	{
		$lowStockLimit = 10;
		$stock = 0;
		
		// Item stock
		if((int) $item->variationBase->limitOrderByStockSelect == 1 && (int) $item->variationStock->stockNet <= 0)
		{
			$stock = 0;
		}
		elseif(	(int) $item->variationBase->limitOrderByStockSelect == 1 &&
				$lowStockLimit > 0 &&
				(int) $item->variationStock->stockNet > 0 &&
				(int) $item->variationStock->stockNet <= $lowStockLimit
		)
		{
			$stock = 1;
		}
		elseif((int) $item->variationStock->stockNet > 0)
		{
			$stock = (int) $item->variationStock->stockNet;
		}
		else
		{
			$stock = $lowStockLimit;
		}

		return $stock;
	}
}

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
	 * @var Map<int, array<int, string>>
	 */
	private Vector<string> $variations = Vector{};

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
				$variationAttributes = $this->getVariationAttributes($item, $settings);

				if($this->handled($item->itemBase->id, $variationAttributes))
				{
					continue;
				}

				$itemName = strlen($this->elasticExportHelper->getName($item, $settings, 256)) <= 0 ? $item->variationBase->id : $this->elasticExportHelper->getName($item, $settings, 256);
				$rrp = $item->variationRecommendedRetailPrice->price > $this->elasticExportHelper->getPrice($item, $settings) ? $item->variationRecommendedRetailPrice->price : $this->elasticExportHelper->getPrice($item, $settings);
				$price = $item->variationRecommendedRetailPrice->price > $this->elasticExportHelper->getPrice($item, $settings) ? $this->elasticExportHelper->getPrice($item, $settings) : $item->variationRecommendedRetailPrice->price;

				$data = [
					'Identnummer'					=> $item->variationBase->id,
					'Artikelnummer'					=> $item->variationBase->customNumber,
					'Herstellerartikelnummer'		=> $item->variationBase->model,
					'Artikelname'					=> $itemName,
					'Artikelbeschreibung'			=> $this->elasticExportHelper->getDescription($item, $settings, 256),
					'Bild' . '(er)'					=> $this->getImages($item, $settings, ';'),
					'360 Grad'						=> $this->getProperty($item, $settings, '360_view_url'),
					'Bestand'						=> $this->getStock($item, $settings),
					'Farbe'							=> $this->getProperty($item, $settings, 'color'),
					'Farbe Suche I'					=> $this->getProperty($item, $settings, 'color_1'),
					'Farbe Suche II'				=> $this->getProperty($item, $settings, 'color_2'),
					'Hersteller Farbbezeichnung'	=> $this->getProperty($item, $settings, 'producer_color'),
					'GG Größengang'					=> $this->getProperty($item, $settings, 'size_range'),
					'Größe'							=> $this->getProperty($item, $settings, 'size'),
					'Marke'							=> $item->itemBase->producer,
					'Saison'						=> $this->getProperty($item, $settings, 'season'),
					'EAN'							=> $item->variationBarcode->code,
					'Währung'						=> $settings->get('currency'),
					'Versandkosten'					=> number_format($this->elasticExportHelper->getShippingCost($item, $settings), 2, '.', ''),
					'Info Versandkosten'			=> $this->getProperty($item, $settings, 'shipping_costs_info'),
					'Preis' . ' (UVP)'				=> number_format($rrp, 2, '.', ''),
					'reduzierter Preis'				=> number_format($price, 2, '.', ''),
					'Grundpreis'					=> '', // TODO
					'Grundpreis Einheit'			=> '', // TODO
					'Kategorien'					=> $this->getCategories($item, $settings),
					'Link'							=> $this->elasticExportHelper->getUrl($item, $settings),
					'Anzahl Verkäufe'				=> $this->getProperty($item, $settings, 'sold_items'),
					'Schuhbreite'					=> $this->getProperty($item, $settings, 'shoe_width'),
					'Absatzhöhe'					=> $this->getProperty($item, $settings, 'heel_height'),
					'Absatzform'					=> $this->getProperty($item, $settings, 'heel_form'),
					'Schuhspitze'					=> $this->getProperty($item, $settings, 'shoe_tip'),
					'Obermaterial'					=> $this->getProperty($item, $settings, 'upper_material'),
					'Schaftweite'					=> $this->getProperty($item, $settings, 'calf_size'),
					'Schafthöhe'					=> $this->getProperty($item, $settings, 'calf_height'),
					'Materialzusammensetzung' 		=> $this->getProperty($item, $settings, 'material_composition'),
					'Besonderheiten'				=> $this->getProperty($item, $settings, 'features'),
					'Verschluss'					=> $this->getProperty($item, $settings, 'fastener'),
					'Innenmaterial'					=> $this->getProperty($item, $settings, 'interior_material'),
					'Sohle'							=> $this->getProperty($item, $settings, 'sole'),
					'Größenhinweis'					=> $this->getProperty($item, $settings, 'size_advice'),
					'Wechselfussbett'				=> $this->getProperty($item, $settings, 'removable_insole'),
					'Wasserdicht'					=> $this->getProperty($item, $settings, 'waterproof'),
					'Promotion'						=> $this->getProperty($item, $settings, 'promotion'),
					'URL Video'						=> $this->getProperty($item, $settings, 'video_url'),
					'Steuersatz'					=> $this->getProperty($item, $settings, 'tax'),
					'ANWR schuh Trend'				=> $this->getProperty($item, $settings, 'shoe_trend'),
				];

				$this->addCSVContent(array_values($data));
			}
		}
	}

	/**
	 * Get property.
	 * @param  Record   $item
	 * @param  KeyValue $settings
	 * @param  string   $property
	 * @return string
	 */
	private function getProperty(Record $item, KeyValue $settings, string $property):string
	{
		$variationAttributes = $this->getVariationAttributes($item, $settings);

		if(array_key_exists($property, $variationAttributes))
		{
			return $variationAttributes[$property];
		}

		$itemPropertyList = $this->getItemPropertyList($item, $settings);

		if(array_key_exists($property, $itemPropertyList))
		{
			return $itemPropertyList[$property];
		}

		return '';
	}


	/**
	 * Get item properties.
	 * @param 	Record $item
	 * @param  KeyValue $settings
	 * @return array<string,string>
	 */
	private function getItemPropertyList(Record $item, KeyValue $settings):array<string,string>
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

	/**
	 * Get variation attributes.
	 * @param  Record   $item
	 * @param  KeyValue $settings
	 * @return array<string,string>
	 */
	private function getVariationAttributes(Record $item, KeyValue $settings):array<string,string>
	{
		$variationAttributes = [];

		foreach($item->variationAttributeValueList as $variationAttribute)
		{
			$attributeValueLang = $this->attributeValueLangRepository->findAttributeValue($variationAttribute->attributeValueId, $settings->get('lang'));

			if($attributeValueLang instanceof AttributeValueLang)
			{
				if($attributeValueLang->attributeValue->attribute->amazon_variation == 'Color')
				{
					$variationAttributes['color'][] = $attributeValueLang->name;
				}

				if($attributeValueLang->attributeValue->attribute->amazon_variation == 'Size')
				{
					$variationAttributes['size'][] = $attributeValueLang->name;
				}
			}
		}

		$list = [];

		foreach($variationAttributes as $key => $value)
		{
			if(is_array($value) && count($value))
			{
				$list[$key] = implode(', ', $value);
			}
		}

		return $list;
	}

	/**
	 * Check if attributes were already handled.
	 * @param  int $itemId
	 * @param  array<string,string>$variationAttributes
	 * @return bool
	 */
	private function handled(int $itemId, array<string,string>$variationAttributes):bool
	{
		$attributes = $this->hashAttributes($itemId, $variationAttributes);

		if(in_array($attributes, $this->variations->toArray()))
		{
			return true;
		}

		$this->variations->add($attributes);

		return false;
	}

	/**
	 * Generate attributes hash.
	 * @param  int $itemId
	 * @param  array<string,string>$variationAttributes
	 * @return string
	 */
	private function hashAttributes(int $itemId, array<string,string>$variationAttributes):string
	{
		$attributes = (string) $itemId;

		if(count($variationAttributes))
		{
			$attributes .= implode(';', $variationAttributes);
		}

		return $attributes;
	}

	/**
	 * Get list of categories
	 * @param  Record $item
	 * @param  KeyValue $settings
	 * @return string
	 */
	private function getCategories(Record $item, KeyValue $settings):string
	{
		$list = [];

		foreach($item->variationCategoryList as $category)
		{
			$category = $this->elasticExportHelper->getCategory($category->categoryId, $settings->get('lang'), $settings->get('plentyId'));

			if(strlen($category))
			{
				$list[] = $category;
			}
		}

		return implode(';', $list);
	}

	/**
     * Get images.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  string   $separator  = ','
     * @param  string   $imageType  = 'normal'
     * @return string
     */
    public function getImages(Record $item, KeyValue $settings, string $separator = ',', string $imageType = 'normal'):string
    {
        $list = $this->elasticExportHelper->getImageList($item, $settings, $imageType);

        if(count($list))
        {
            return implode($separator, $list);
        }

        return '';
    }
}

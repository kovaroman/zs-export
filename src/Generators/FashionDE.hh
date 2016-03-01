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

class FashionDE extends CSVGenerator
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
     * Billiger constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     * @param AttributeValueLangRepositoryContract $attributeValueLangRepository,
     */
    public function __construct(ElasticExportHelper $elasticExportHelper, ArrayHelper $arrayHelper, AttributeValueLangRepositoryContract $attributeValueLangRepository)
    {
        $this->elasticExportHelper = $elasticExportHelper;
		$this->arrayHelper = $arrayHelper;
		$this->attributeValueLangRepository = $attributeValueLangRepository;
    }

	/**
	 * @param mixed $resultData
	 */
	protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
	{
        if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(" ");

			$this->addCSVContent([
					'art_nr',
					'art_name',
					'art_kurztext',
					'art_kategorie',
					'art_url',
					'art_img_url',
					'waehrung',
					'art_preis',

					'art_marke',
					'art_farbe',
					'art_groesse',
					'art_versand',
					'art_sale_preis',
					'art_geschlecht',
					'art_grundpreis',

			]);

			$rows = [];

			foreach($resultData as $item)
			{
				if(!array_key_exists($item->itemBase->id, $rows))
				{
					$rows[$item->itemBase->id] = $this->getMain($item, $settings);
				}

				if(array_key_exists($item->itemBase->id, $rows) && $item->variationBase->attributeValueSetId > 0)
				{
					$variationAttributes = $this->getVariationAttributes($item, $settings);

					if(array_key_exists('Color', $variationAttributes))
					{
						$rows[$item->itemBase->id]['art_farbe'] = array_unique(array_merge($rows[$item->itemBase->id]['art_farbe'], $variationAttributes['Color']));
					}

					if(array_key_exists('Size', $variationAttributes))
					{
						$rows[$item->itemBase->id]['art_groesse'] = array_unique(array_merge($rows[$item->itemBase->id]['art_groesse'], $variationAttributes['Size']));
					}
				}
			}

			foreach($rows as $data)
			{
				if(array_key_exists('art_farbe', $data) && is_array($data['art_farbe']))
				{
					$data['art_farbe'] = implode(', ', $data['art_farbe']);
				}

				if(array_key_exists('art_groesse', $data) && is_array($data['art_groesse']))
				{
					$data['art_groesse'] = implode(', ', $data['art_groesse']);
				}

				$this->addCSVContent(array_values($data));
			}
		}
    }

	/**
	 * Get main information.
	 * @param  Record   $item
	 * @param  KeyValue $settings
	 * @return array<string,mixed>
	 */
	private function getMain(Record $item, KeyValue $settings):array<string,mixed>
	{
		$price = $item->variationRecommendedRetailPrice->price > $this->elasticExportHelper->getPrice($item, $settings) ? $item->variationRecommendedRetailPrice->price : $this->elasticExportHelper->getPrice($item, $settings);
		$rrp = $item->variationRecommendedRetailPrice->price > $this->elasticExportHelper->getPrice($item, $settings) ? $this->elasticExportHelper->getPrice($item, $settings) : $item->variationRecommendedRetailPrice->price;

		$data = [
			'art_nr' 			=> $item->itemBase->id,
			'art_name' 			=> $this->elasticExportHelper->getName($item, $settings),
			'art_kurztext' 		=> $this->elasticExportHelper->getDescription($item, $settings, 3000),
			'art_kategorie' 	=> $this->elasticExportHelper->getCategory($item, $settings),
			'art_url' 			=> $this->elasticExportHelper->getUrl($item, $settings),
			'art_img_url' 		=> $this->elasticExportHelper->getMainImage($item, $settings),
			'waehrung' 			=> $this->elasticExportHelper->getDefaultCurrency(),
			'art_preis' 		=> number_format($price, 2, ',', ''),
			'art_marke'			=> substr(trim($item->itemBase->producer), 0, 20),
			'art_groesse' 		=> [],
			'art_farbe'			=> [],
			'art_versand'		=> $this->elasticExportHelper->getShippingCost($item, $settings),
			'art_sale_preis' 	=> number_format($rrp, 2, ',', ''),
			'art_geschlecht' 	=> $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, 'article_gender'),
			'art_grundpreis'	=> $this->elasticExportHelper->getBasePrice($item, $settings, '/', false, false, '', $rrp > 0 ? $rrp : $price),
		];

		return $data;
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
				$amazonVariation = $attributeValueLang->attributeValue->attribute->amazon_variation == 'Color';
				if($attributeValueLang->attributeValue->attribute->amazon_variation)
				{
					$variationAttributes[$attributeValueLang->attributeValue->attribute->amazon_variation][] = $attributeValueLang->name;
				}
			}
		}

		return $variationAttributes;
	}
}
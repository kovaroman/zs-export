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
use Plenty\Modules\Item\Character\Contracts\CharacterSelectionRepositoryContract;
use Plenty\Modules\Item\Character\Models\CharacterSelection;


class Shopping24DE extends CSVGenerator
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
     * Shopping24DE constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     * @param AttributeValueLangRepositoryContract $attributeValueLangRepository
     * @param CharacterSelectionRepositoryContract $characterSelectionRepository
     */
    public function __construct(ElasticExportHelper $elasticExportHelper,
                                ArrayHelper $arrayHelper,
                                AttributeValueLangRepositoryContract $attributeValueLangRepository,
                                CharacterSelectionRepositoryContract $characterSelectionRepository)
    {
        $this->elasticExportHelper          = $elasticExportHelper;
        $this->arrayHelper                  = $arrayHelper;
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

			$this->setDelimiter(" ");

			$this->addCSVContent([
					'art_name',
                    'long_description',
                    'image_url',
                    'deep_link',
                    'price',
                    'old_price',
                    'currency',
                    'delivery_costs',
                    'category',
                    'brand',
                    'gender_age',
                    'ean',
                    'keywords',
                    'art_number',
                    'color',
                    'clothing_size',
                    'cut',
                    'link',
                    'unit_price'
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
                        $rows[$item->itemBase->id]['color'] = array_unique(array_merge($rows[$item->itemBase->id]['color'], $variationAttributes['Color']));
                    }

                    if(array_key_exists('Size', $variationAttributes))
                    {
                        $rows[$item->itemBase->id]['clothing_size'] = array_unique(array_merge($rows[$item->itemBase->id]['clothing_size'], $variationAttributes['Size']));
                    }
                }
            }

            foreach($rows as $data)
            {
                if(array_key_exists('color', $data) && is_array($data['color']))
                {
                    $data['color'] = implode(', ', $data['color']);
                }

                if(array_key_exists('clothing_size', $data) && is_array($data['clothing_size']))
                {
                    $data['clothing_size'] = implode(', ', $data['clothing_size']);
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
        $rrp = $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) > $this->elasticExportHelper->getPrice($item) ? $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) : '';

        $data = [
            'art_name'          => strip_tags(html_entity_decode($this->elasticExportHelper->getName($item, $settings, 80))),
            'long_description'  => preg_replace(array("/\t/","/;/","/\|/"),"",strip_tags(html_entity_decode($this->elasticExportHelper->getDescription($item, $settings)))),
            'image_url'         => $this->elasticExportHelper->getMainImage($item, $settings),
            'deep_link'         => $this->elasticExportHelper->getUrl($item, $settings, true, false),
            'price'             => number_format($this->elasticExportHelper->getPrice($item), 2, ',', ''),
            'old_price'         => number_format($rrp, 2, ',',''),
            'currency'          => $item->variationRetailPrice->currency,
            'delivery_costs'    => number_format($this->elasticExportHelper->getShippingCost($item, $settings), 2, ',', ''),
            'category'          => $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
            'brand'             => html_entity_decode($item->itemBase->producer),
            'gender_age'        => $this->itemPropertyCache[$item->itemBase->id]['gender_age'],
            'ean'               => $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_EAN),
            'keywords'          => html_entity_decode($item->itemDescription->keywords),
            'art_number'        => html_entity_decode($item->variationBase->customNumber),
            'color'             => [],
            'clothing_size'     => [],
            'cut'               => '',
            'link'              => '',
            'unit_price'        => $this->elasticExportHelper->getBasePrice($item, $settings),
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
                if($attributeValueLang->attributeValue->attribute->amazon_variation)
                {
                    $variationAttributes[$attributeValueLang->attributeValue->attribute->amazon_variation][] = $attributeValueLang->name;
                }
            }
        }

        return $variationAttributes;
    }

    /**
     * Get item properties.
     * @param 	Record $item
     * @return array<string,string>
     */
	protected function getItemPropertyList(Record $item):array<string,string>
	{
        if(!array_key_exists($item->itemBase->id, $this->itemPropertyCache))
        {
            $characterMarketComponentList = $this->elasticExportHelper->getItemCharactersByComponent($item, 146.00);

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

}

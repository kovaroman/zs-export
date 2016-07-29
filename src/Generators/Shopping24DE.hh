<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Item\Attribute\Contracts\AttributeValueNameRepositoryContract;
use Plenty\Modules\Item\Attribute\Models\AttributeValueName;
use Plenty\Modules\Item\Property\Contracts\PropertySelectionRepositoryContract;
use Plenty\Modules\Item\Property\Models\PropertySelection;


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
     * AttributeValueNameRepositoryContract $attributeValueNameRepository
     */
    private AttributeValueNameRepositoryContract $attributeValueNameRepository;

    /**
     * PropertySelectionRepositoryContract $propertySelectionRepository
     */
    private PropertySelectionRepositoryContract $propertySelectionRepository;

    /**
     * @var array<int,mixed>
     */
    private array<int,array<string,string>>$itemPropertyCache = [];

    /**
     * Shopping24DE constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     * @param AttributeValueNameRepositoryContract $attributeValueNameRepository
     * @param PropertySelectionRepositoryContract $propertySelectionRepository
     */
    public function __construct(ElasticExportHelper $elasticExportHelper,
                                ArrayHelper $arrayHelper,
                                AttributeValueNameRepositoryContract $attributeValueNameRepository,
                                PropertySelectionRepositoryContract $propertySelectionRepository)
    {
        $this->elasticExportHelper          = $elasticExportHelper;
        $this->arrayHelper                  = $arrayHelper;
        $this->attributeValueNameRepository = $attributeValueNameRepository;
        $this->propertySelectionRepository = $propertySelectionRepository;
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
            $attributeValueName = $this->attributeValueNameRepository->findOne($variationAttribute->attributeValueId, $settings->get('lang'));

            if($attributeValueName instanceof AttributeValueName)
            {
                if($attributeValueName->attributeValue->attribute->amazon_variation)
                {
                    $variationAttributes[$attributeValueName->attributeValue->attribute->amazon_variation][] = $attributeValueName->name;
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
                            $propertySelection = $this->propertySelectionRepository->findOne((int) $data['characterValue'], 'de');
                            if($propertySelection instanceof PropertySelection)
                            {
                                $list[(string) $data['externalComponent']] = (string) $propertySelection->name;
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

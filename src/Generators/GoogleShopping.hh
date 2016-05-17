<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Item\Unit\Models\UnitLang;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Item\Attribute\Contracts\AttributeValueLangRepositoryContract;
use Plenty\Modules\Item\Attribute\Models\AttributeValueLang;
use Plenty\Modules\Item\Character\Contracts\CharacterSelectionRepositoryContract;
use Plenty\Modules\Item\Character\Models\CharacterSelection;


class GoogleShopping extends CSVGenerator
{
    const string CHARACTER_TYPE_GENDER						= 'gender';
    const string CHARACTER_TYPE_AGE_GROUP					= 'age_group';
    const string CHARACTER_TYPE_SIZE_TYPE					= 'size_type';
    const string CHARACTER_TYPE_SIZE_SYSTEM					= 'size_system';
    const string CHARACTER_TYPE_ENERGY_EFFICIENCY_CLASS		= 'energy_efficiency_class';
    const string CHARACTER_TYPE_EXCLUDED_DESTINATION		= 'excluded_destination';
    const string CHARACTER_TYPE_ADWORDS_REDIRECT			= 'adwords_redirect';
    const string CHARACTER_TYPE_MOBILE_LINK					= 'mobile_link';
    const string CHARACTER_TYPE_SALE_PRICE_EFFECTIVE_DATE	= 'sale_price_effective_date';
    const string CHARACTER_TYPE_CUSTOM_LABEL_0				= 'custom_label_0';
    const string CHARACTER_TYPE_CUSTOM_LABEL_1				= 'custom_label_1';
    const string CHARACTER_TYPE_CUSTOM_LABEL_2				= 'custom_label_2';
    const string CHARACTER_TYPE_CUSTOM_LABEL_3				= 'custom_label_3';
    const string CHARACTER_TYPE_CUSTOM_LABEL_4				= 'custom_label_4';
    const string CHARACTER_TYPE_DESCRIPTION					= 'description';
    const string CHARACTER_TYPE_COLOR						= 'color';
    const string CHARACTER_TYPE_SIZE						= 'size';
    const string CHARACTER_TYPE_PATTERN						= 'pattern';
    const string CHARACTER_TYPE_MATERIAL					= 'material';

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
     * @var ImmMap<int,string>
     */
    private ImmMap<int,string> $itemAvailability = ImmMap{
                                                        0 => 'in stock',
                                                        1 => 'in stock',
                                                        2 => 'out of stock',
                                                        3 => 'preorder'
                                                    };

    /**
         * GoogleShopping constructor.
         * @param ElasticExportHelper $elasticExportHelper
         * @param ArrayHelper $arrayHelper
         */
    public function __construct(
        ElasticExportHelper $elasticExportHelper, ArrayHelper $arrayHelper,
        AttributeValueLangRepositoryContract $attributeValueLangRepository,
        CharacterSelectionRepositoryContract $characterSelectionRepository)
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

			$this->setDelimiter("	"); // this is tab character!

			$this->addCSVContent([
				'id',
				'title',
				'description',
				'google_product_category',
				'product_type',
				'link',
				'image_link',
				'condition',
				'availability',
				'price',
				'sale_price',
				'brand',
				'gtin',
				'isbn',
				'mpn',
				'color',
				'size',
				'material',
				'pattern',
				'item_group_id',
				'shipping',
				'shipping_weight',
				'gender',
				'age_group',
				'excluded_destination',
				'adwords_redirect',
				'identifier_exists',
				'unit_pricing_measure',
				'unit_pricing_base_measure',
				'energy_efficiency_class',
				'size_system',
				'size_type',
				'mobile_link',
				'sale_price_effective_date',
				'adult',
				'custom_label_0',
				'custom_label_1',
				'custom_label_2',
				'custom_label_3',
				'custom_label_4',
			]);

            $rows = [];

			foreach($resultData as $item)
			{
                $variationAttributes = $this->getVariationAttributes($item, $settings);
                $variationPrice = number_format($this->elasticExportHelper->getPrice($item), 2, '.', '');
                $salePrice = number_format($this->elasticExportHelper->getSpecialPrice($item, $settings), 2, '.', '');
                if($salePrice >= $variationPrice)
                {
                    $salePrice = '';
                }

				$data = [
					'id' 						=> $item->variationBase->id,
					'title' 					=> $this->elasticExportHelper->getName($item, $settings, 256),
					'description'				=> $this->getDescription($item, $settings),
					'google_product_category'	=> '',
					'product_type'				=> $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
					'link'						=> $this->elasticExportHelper->getUrl($item, $settings, true, false),
					'image_link'				=> $this->elasticExportHelper->getMainImage($item, $settings),
					'condition'					=> $this->getCondition($item->itemBase->condition),
					'availability'				=> $this->itemAvailability->get((int)$this->elasticExportHelper->getAvailability($item, $settings, false)),
					'price'						=> $variationPrice,
					'sale_price'				=> $salePrice,
					'brand'						=> $item->itemBase->producer,
					'gtin'						=> $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_EAN),
					'isbn'						=> $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_ISBN),
					'mpn'						=> $item->variationBase->model,
					'color'						=> $variationAttributes['color'],
					'size'						=> $variationAttributes['size'],
					'material'					=> $variationAttributes['material'],
					'pattern'					=> $variationAttributes['pattern'],
					'item_group_id'				=> $item->itemBase->id,
					'shipping'					=> number_format($this->elasticExportHelper->getShippingCost($item, $settings), 2, ',', ''),
					'shipping_weight'			=> $item->variationBase->weightG.' g',
					'gender'					=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_GENDER),
					'age_group'					=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_AGE_GROUP),
					'excluded_destination'		=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_EXCLUDED_DESTINATION),
					'adwords_redirect'			=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_ADWORDS_REDIRECT),
					'identifier_exists'			=> $this->getIdentifierExists($item),
					'unit_pricing_measure'		=> $this->getUnitPricingMeasure($item, $settings),
					'unit_pricing_base_measure'	=> $this->getUnitPricingBaseMeasure($item, $settings),
					'energy_efficiency_class'	=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_ENERGY_EFFICIENCY_CLASS),
					'size_system'				=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_SIZE_SYSTEM),
					'size_type'					=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_SIZE_TYPE),
					'mobile_link'				=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_MOBILE_LINK),
					'sale_price_effective_date'	=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_SALE_PRICE_EFFECTIVE_DATE),
					'adult'						=> '',
					'custom_label_0'			=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_CUSTOM_LABEL_0),
					'custom_label_1'			=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_CUSTOM_LABEL_1),
					'custom_label_2'			=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_CUSTOM_LABEL_2),
					'custom_label_3'			=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_CUSTOM_LABEL_3),
					'custom_label_4'			=> $this->getProperty($item, $settings, self::CHARACTER_TYPE_CUSTOM_LABEL_4),
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

        switch ($property)
        {
            case self::CHARACTER_TYPE_GENDER:
                $allowedList = [
                    'male',
                    'female',
                    'unisex',
                ];
                break;

            case self::CHARACTER_TYPE_AGE_GROUP:
                $allowedList = [
                    'newborn',
                    'infant',
                    'toddler',
                    'adult',
                    'kids',
                ];
                break;

            case self::CHARACTER_TYPE_SIZE_TYPE:
                $allowedList = [
                    'regular',
                    'petite',
                    'plus',
                    'maternity',
                ];
                break;

            case self::CHARACTER_TYPE_SIZE_SYSTEM:
                $allowedList = [
                    'US',
                    'UK',
                    'EU',
                    'DE',
                    'FR',
                    'JP',
                    'CN',
                    'IT',
                    'BR',
                    'MEX',
                    'AU',
                ];
                break;

            case self::CHARACTER_TYPE_ENERGY_EFFICIENCY_CLASS:
                $allowedList = [
                    'G',
                    'F',
                    'E',
                    'D',
                    'C',
                    'B',
                    'A',
                    'A+',
                    'A++',
                    'A+++',
                ];
                break;
        }

        if(array_key_exists($property, $itemPropertyList) && in_array($itemPropertyList[$property], $allowedList))
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
     * Check if condition is valid.
     * @param int $condition
     * @return string
     */
    private function getCondition(int $conditionId):string
    {
        $conditionList = [
            0 => 'new',
            1 => 'used',
            2 => 'new',
            3 => 'new',
            4 => 'refurbished',
        ];

        if (array_key_exists($conditionId, $conditionList))
        {
            return $conditionList[$conditionId];
        }
        else
        {
            return '';
        }
    }

    /**
     * Calculate and get unit price
     * @param Record $item
     * @return string
     */
    private function getIdentifierExists(Record $item):string
    {
        $count = 0;
        if (strlen($item->variationBase->model) > 0)
        {
            $count++;
        }

        if (strlen($item->variationBarcode->code) > 0)
        {
            $count++;
        }

        if (strlen($item->itemBase->producer) > 0)
        {
            $count++;
        }

        if ($count >= 2)
        {
            return 'true';
        }
        else
        {
            return 'false';
        }
    }

    /**
     * Calculate and get unit price
     * @param Record $item
     * @param KeyValue $settings
     * @return string
     */
    private function getUnitPricingBaseMeasure(Record $item, KeyValue $settings):string
    {
        $basePriceList = $this->elasticExportHelper->getBasePriceList($item, $settings);
        return (string)$basePriceList['lot'].' '.(string)$basePriceList['unit'];
    }

    /**
     * Calculate and get unit price
     * @param Record $item
     * @param KeyValue $settings
     * @return string
     */
    private function getUnitPricingMeasure(Record $item, KeyValue $settings):string
    {
        $basePriceList = $this->elasticExportHelper->getBasePriceList($item, $settings);
        return (string)number_format($item->variationBase->content, 2, '.', '').' '.(string)$basePriceList['unit'];
    }

//    /**
//     * Check if gender is valid.
//     * @param Record $item
//     * @param KeyValue $settings
//     * @param string $type
//     * @return string
//     */
//    private function getCharacterValue(Record $item, KeyValue $settings, string $type):string
//    {
//        $characterValue = $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, $type);
//
//        switch ($type)
//        {
//            case self::CHARACTER_TYPE_GENDER:
//                $allowedList = [
//                    'male',
//                    'female',
//                    'unisex',
//                ];
//                break;
//
//            case self::CHARACTER_TYPE_AGE_GROUP:
//                $allowedList = [
//                    'newborn',
//                    'infant',
//                    'toddler',
//                    'adult',
//                    'kids',
//                ];
//                break;
//
//            case self::CHARACTER_TYPE_SIZE_TYPE:
//                $allowedList = [
//                    'regular',
//                    'petite',
//                    'plus',
//                    'maternity',
//                ];
//                break;
//
//            case self::CHARACTER_TYPE_SIZE_SYSTEM:
//                $allowedList = [
//                    'US',
//                    'UK',
//                    'EU',
//                    'DE',
//                    'FR',
//                    'JP',
//                    'CN',
//                    'IT',
//                    'BR',
//                    'MEX',
//                    'AU',
//                ];
//                break;
//
//            case self::CHARACTER_TYPE_ENERGY_EFFICIENCY_CLASS:
//                $allowedList = [
//                    'G',
//                    'F',
//                    'E',
//                    'D',
//                    'C',
//                    'B',
//                    'A',
//                    'A+',
//                    'A++',
//                    'A+++',
//                ];
//                break;
//        }
//
//        if (in_array($characterValue, $allowedList))
//        {
//            return $characterValue;
//        }
//        else
//        {
//            return '';
//        }
//    }

    /**
     * Get item description.
     * @param Record $item
     * @param KeyValue $settings
     * @return string
     */
    private function getDescription(Record $item, KeyValue $settings):string
    {
        $description = $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, self::CHARACTER_TYPE_DESCRIPTION);

        if (strlen($description) <= 0)
        {
            $description = $this->elasticExportHelper->getDescription($item, $settings, 5000);
        }

        return $description;
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
                if($attributeValueLang->attributeValue->attribute->googleproducts_variation)
                {
                    $variationAttributes[$attributeValueLang->attributeValue->attribute->googleproducts_variation][] = $attributeValueLang->name;
                }
            }
        }

        return $variationAttributes;
    }
}

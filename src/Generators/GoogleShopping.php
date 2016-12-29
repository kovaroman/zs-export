<?php

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Item\Attribute\Contracts\AttributeRepositoryContract;
use Plenty\Modules\Item\Attribute\Models\Attribute;
use Plenty\Modules\Item\Attribute\Contracts\AttributeValueNameRepositoryContract;
use Plenty\Modules\Item\Attribute\Models\AttributeValueName;
use Plenty\Modules\Item\Attribute\Contracts\AttributeValueRepositoryContract;
use Plenty\Modules\Item\Attribute\Models\AttributeValue;
use Plenty\Modules\Item\Property\Contracts\PropertySelectionRepositoryContract;
use Plenty\Modules\Item\Property\Models\PropertySelection;
use Plenty\Repositories\Models\PaginatedResult;

class GoogleShopping extends CSVGenerator
{
    const CHARACTER_TYPE_GENDER						= 'gender';
    const CHARACTER_TYPE_AGE_GROUP					= 'age_group';
    const CHARACTER_TYPE_SIZE_TYPE					= 'size_type';
    const CHARACTER_TYPE_SIZE_SYSTEM				= 'size_system';
    const CHARACTER_TYPE_ENERGY_EFFICIENCY_CLASS	= 'energy_efficiency_class';
    const CHARACTER_TYPE_EXCLUDED_DESTINATION		= 'excluded_destination';
    const CHARACTER_TYPE_ADWORDS_REDIRECT			= 'adwords_redirect';
    const CHARACTER_TYPE_MOBILE_LINK			    = 'mobile_link';
    const CHARACTER_TYPE_SALE_PRICE_EFFECTIVE_DATE	= 'sale_price_effective_date';
    const CHARACTER_TYPE_CUSTOM_LABEL_0				= 'custom_label_0';
    const CHARACTER_TYPE_CUSTOM_LABEL_1				= 'custom_label_1';
    const CHARACTER_TYPE_CUSTOM_LABEL_2				= 'custom_label_2';
    const CHARACTER_TYPE_CUSTOM_LABEL_3				= 'custom_label_3';
    const CHARACTER_TYPE_CUSTOM_LABEL_4				= 'custom_label_4';
    const CHARACTER_TYPE_DESCRIPTION					= 'description';
    const CHARACTER_TYPE_COLOR						= 'color';
    const CHARACTER_TYPE_SIZE						= 'size';
    const CHARACTER_TYPE_PATTERN						= 'pattern';
    const CHARACTER_TYPE_MATERIAL					= 'material';

    const ISO_CODE_2                                = 'isoCode2';
    const ISO_CODE_3                                = 'isoCode3';

	/*
	 * @var ElasticExportHelper
	 */
    private $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
    private $arrayHelper;

	/**
	 * AttributeValueNameRepositoryContract $attributeValueNameRepository
	 */
    private $attributeValueNameRepository;

	/**
	 * AttributeRepositoryContract $attributeRepository
	 */
	private $attributeRepository;

	/**
	 * AttributeValueRepositoryContract $attributeValueRepository
	 */
	private $attributeValueRepository;

    /**
     * PropertySelectionRepositoryContract $propertySelectionRepository
     */
    private $propertySelectionRepository;

    /**
     * @var array
     */
    private $itemPropertyCache = [];

	/**
	 * @var array
	 */
	private $attributeValueCache = [];

	/**
	 * @var array
	 */
	private $linkedAttributeList = [];

	/**
	 * GoogleShopping constructor.
	 * @param ElasticExportHelper $elasticExportHelper
	 * @param ArrayHelper $arrayHelper
	 * @param AttributeRepositoryContract $attributeRepository
	 * @param AttributeValueRepositoryContract $attributeValueRepository
	 * @param AttributeValueNameRepositoryContract $attributeValueNameRepository
	 * @param PropertySelectionRepositoryContract $propertySelectionRepository
	 */
    public function __construct(
        ElasticExportHelper $elasticExportHelper, ArrayHelper $arrayHelper,
        AttributeValueNameRepositoryContract $attributeValueNameRepository,
        PropertySelectionRepositoryContract $propertySelectionRepository,
		AttributeValueRepositoryContract $attributeValueRepository,
		AttributeRepositoryContract $attributeRepository)
    {
        $this->elasticExportHelper = $elasticExportHelper;
        $this->arrayHelper = $arrayHelper;
        $this->attributeValueNameRepository = $attributeValueNameRepository;
        $this->propertySelectionRepository = $propertySelectionRepository;
		$this->attributeValueRepository = $attributeValueRepository;
		$this->attributeRepository = $attributeRepository;
    }

    /**
     * @param RecordList $resultData
     */
    protected function generateContent($resultData, array $formatSettings = [])
	{
		if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');
			$this->loadLinkedAttributeList($settings);
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


			foreach($resultData as $item)
			{
                $variationAttributes = $this->getVariationAttributes($item);
                $variationPrice = number_format((float)$this->elasticExportHelper->getPrice($item), 2, '.', '');
                $salePrice = number_format((float)$this->elasticExportHelper->getSpecialPrice($item, $settings), 2, '.', '');

                if($salePrice >= $variationPrice || $salePrice <= 0.00)
                {
                    $salePrice = '';
                }

                $shippingCost = $this->elasticExportHelper->getShippingCost($item, $settings);

                if(!is_null($shippingCost))
                {
                    $shippingCost = number_format((float)$shippingCost, 2, '.', '');
                }
                else
                {
                    $shippingCost = '';
                }

                if(strlen($shippingCost) == 0)
                {
                    $shipping = '';
                }
                else
                {
                    $shipping = $this->elasticExportHelper->getCountry($settings, self::ISO_CODE_2).':::'.$shippingCost;
                }


				$data = [
					'id' 						=> $item->variationBase->id,
					'title' 					=> $this->elasticExportHelper->getName($item, $settings, 256),
					'description'				=> $this->getDescription($item, $settings),
					'google_product_category'	=> $this->elasticExportHelper->getCategoryMarketplace((int)$item->variationStandardCategory->categoryId, (int)$settings->get('plentyId'), 129),
					'product_type'				=> $this->elasticExportHelper->getCategory((int)$item->variationStandardCategory->categoryId, (string)$settings->get('lang'), (int)$settings->get('plentyId')),
					'link'						=> $this->elasticExportHelper->getUrl($item, $settings, true, false),
					'image_link'				=> $this->elasticExportHelper->getMainImage($item, $settings),
					'condition'					=> $this->getCondition($item->itemBase->condition),
					'availability'				=> $this->elasticExportHelper->getAvailability($item, $settings, false),
					'price'						=> $variationPrice,
					'sale_price'				=> $salePrice,
					'brand'						=> $this->elasticExportHelper->getExternalManufacturerName((int)$item->itemBase->producerId),
					'gtin'						=> $this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode')),
					'isbn'						=> $this->elasticExportHelper->getBarcodeByType($item, ElasticExportHelper::BARCODE_ISBN),
					'mpn'						=> $item->variationBase->model,
					'color'						=> $variationAttributes['color'][0],
					'size'						=> $variationAttributes['size'][0],
					'material'					=> $variationAttributes['material'][0],
					'pattern'					=> $variationAttributes['pattern'][0],
					'item_group_id'				=> $item->itemBase->id,
					'shipping'					=> $shipping,
					'shipping_weight'			=> $item->variationBase->weightG.' g',
					'gender'					=> $this->getProperty($item, self::CHARACTER_TYPE_GENDER),
					'age_group'					=> $this->getProperty($item, self::CHARACTER_TYPE_AGE_GROUP),
					'excluded_destination'		=> $this->getProperty($item, self::CHARACTER_TYPE_EXCLUDED_DESTINATION),
					'adwords_redirect'			=> $this->getProperty($item, self::CHARACTER_TYPE_ADWORDS_REDIRECT),
					'identifier_exists'			=> $this->getIdentifierExists($item, $settings),
					'unit_pricing_measure'		=> (string)number_format((float)$item->variationBase->content, 3, '.', '').' '.
                                                        (string)$this->getUnit($item),
					'unit_pricing_base_measure'	=> $this->getUnitPricingBaseMeasure($item, $settings),
					'energy_efficiency_class'	=> $this->getProperty($item, self::CHARACTER_TYPE_ENERGY_EFFICIENCY_CLASS),
					'size_system'				=> $this->getProperty($item, self::CHARACTER_TYPE_SIZE_SYSTEM),
					'size_type'					=> $this->getProperty($item, self::CHARACTER_TYPE_SIZE_TYPE),
					'mobile_link'				=> $this->getProperty($item, self::CHARACTER_TYPE_MOBILE_LINK),
					'sale_price_effective_date'	=> $this->getProperty($item, self::CHARACTER_TYPE_SALE_PRICE_EFFECTIVE_DATE),
					'adult'						=> '',
					'custom_label_0'			=> $this->getProperty($item, self::CHARACTER_TYPE_CUSTOM_LABEL_0),
					'custom_label_1'			=> $this->getProperty($item, self::CHARACTER_TYPE_CUSTOM_LABEL_1),
					'custom_label_2'			=> $this->getProperty($item, self::CHARACTER_TYPE_CUSTOM_LABEL_2),
					'custom_label_3'			=> $this->getProperty($item, self::CHARACTER_TYPE_CUSTOM_LABEL_3),
					'custom_label_4'			=> $this->getProperty($item, self::CHARACTER_TYPE_CUSTOM_LABEL_4),
				];

				$this->addCSVContent(array_values($data));
			}
        }
    }

    /**
     * Get property.
     * @param  Record   $item
     * @param  string   $property
     * @return string
     */
    private function getProperty(Record $item, string $property):string
    {
        $itemPropertyList = $this->getItemPropertyList($item);

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
     * @return array<string,string>
     */
    private function getItemPropertyList(Record $item):array
	{
        if(!array_key_exists($item->itemBase->id, $this->itemPropertyCache))
        {
            $characterMarketComponentList = $this->elasticExportHelper->getItemCharactersByComponent($item, 129.00);

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
    private function getIdentifierExists(Record $item, KeyValue $settings):string
    {
        $count = 0;
        if (strlen($item->variationBase->model) > 0)
        {
            $count++;
        }

        if (    strlen($this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode'))) > 0 ||
                strlen($this->elasticExportHelper->getBarcodeByType($item, ElasticExportHelper::BARCODE_ISBN)) > 0 )
        {
            $count++;
        }

        if (strlen($this->elasticExportHelper->getExternalManufacturerName($item->itemBase->producerId)) > 0)
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
     * Returns the unit, if there is any unit configured, which is allowed
     * for GoogleShopping.
     *
     * @param  Record   $item
     * @return string
     */
    private function getUnit(Record $item):string
    {
        switch((int) $item->variationBase->unitId)
        {
            case '1':
                return 'ct'; //Stück
            case '32':
                return 'ml'; // Milliliter
            case '5':
                return 'l'; // Liter
            case '4':
                return 'mg'; //Milligramm
            case '3':
                return 'g'; // Gramm
            case '2':
                return 'kg'; // Kilogramm
            case '51':
                return 'cm'; // Zentimeter
            case '31':
                return 'm'; // Meter
            case '38':
                return 'sqm'; // Quadratmeter
            default:
                return '';
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
        return (string)$basePriceList['lot'].' '.(string)$this->getUnit($item);
    }

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
	 * @return array<string,string>
	 */
    private function getVariationAttributes(Record $item):array
    {
		$variationAttributes = [];

		foreach($item->variationAttributeValueList as $variationAttribute)
		{
			if(array_key_exists($variationAttribute->attributeId, $this->linkedAttributeList) && strlen($this->linkedAttributeList[$variationAttribute->attributeId]) > 0)
			{
				if (strlen($this->attributeValueCache[$variationAttribute->attributeValueId]) > 0)
				{
					$variationAttributes[$this->linkedAttributeList[$variationAttribute->attributeId]][] = $this->attributeValueCache[$variationAttribute->attributeValueId];
				}
			}
		}

		return $variationAttributes;
	}

	/**
	 * Get google linkes attribute list.
	 * @param  KeyValue $settings
	 * @return array<string,string>
	 */
	private function loadLinkedAttributeList(KeyValue $settings)
	{
        $attributeListPage = 1;
        $attributeListTotal = null;

		$attributeList = $this->attributeRepository->all();
		if($attributeList instanceof PaginatedResult)
		{
            $attributeListTotal = $attributeList->getTotalCount();

            while($attributeListTotal > 0)
            {
                $this->iterateAttributeList($attributeList, $settings);
                $attributeListPage++;
                $attributeListTotal = $attributeListTotal - 50;
                $attributeList = $this->attributeRepository->all(['*'], 50, $attributeListPage);
            }
		}
	}

    /**
     * @param PaginatedResult $attributeList
     * @param KeyValue $settings
     */
	private function iterateAttributeList($attributeList, $settings)
    {
        foreach($attributeList->getResult() as $attribute)
        {
            $attributeValuePage = 1;
            $attributeValueTotal = null;

            if($attribute instanceof Attribute)
            {
                if(strlen($attribute->googleShoppingAttribute) > 0)
                {
                    $this->linkedAttributeList[$attribute->id] = $attribute->googleShoppingAttribute;

                    $attributeValueList = $this->attributeValueRepository->findByAttributeId($attribute->id, $attributeValuePage);
                    if($attributeValueList instanceof PaginatedResult)
                    {
                        $attributeValueTotal = $attributeValueList->getTotalCount();
                        while($attributeValueTotal > 0)
                        {
                            $this->setAttributeValueCache($attributeValueList, $settings);
                            $attributeValuePage++;
                            $attributeValueTotal = $attributeValueTotal - 50;
                            $attributeValueList = $this->attributeValueRepository->findByAttributeId($attribute->id, $attributeValuePage);
                        }
                    }
                }
            }
        }
    }

    /**
     * @param PaginatedResult $attibuteValueList
     * @param KeyValue $settings
     */
	private function setAttributeValueCache($attributeValueList, $settings)
    {
        if($attributeValueList instanceof PaginatedResult)
        {
            foreach ($attributeValueList->getResult() as $attributeValue)
            {
                $attributeValueName = $this->attributeValueNameRepository->findOne($attributeValue->id, $settings->get('lang'));

                if($attributeValueName instanceof AttributeValueName)
                {
                    $this->attributeValueCache[$attributeValue->id] = $attributeValueName->name;
                }
            }
        }
    }
}

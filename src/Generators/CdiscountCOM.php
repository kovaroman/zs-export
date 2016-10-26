<?php

namespace ElasticExport\Generators;

use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\Attribute\Contracts\AttributeValueNameRepositoryContract;
use Plenty\Modules\Item\Attribute\Models\AttributeValueName;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\Item\Property\Contracts\PropertySelectionRepositoryContract;
use Plenty\Modules\Item\Property\Models\PropertySelection;

/**
 * Class CdiscountCOM
 * @package ElasticExport\Generators
 */
class CdiscountCOM extends CSVGenerator
{
    const CHARACTER_TYPE_DESCRIPTION					=   'description';
    const CHARACTER_TYPE_GENDER                      =   'gender';
    const CHARACTER_TYPE_TYPE_OF_PUBLIC              =   'type_of_public';
    const CHARACTER_TYPE_SPORTS                      =   'sports';
    const CHARACTER_TYPE_WARNINGS                    =   'warnings';
    const CHARACTER_TYPE_COMMENT                     =   'comment';
    const CHARACTER_TYPE_MAIN_COLOR                  =   'main_color';
    const CHARACTER_TYPE_MARKETING_DESCRIPTION       =   'marketing_description';
    const CHARACTER_TYPE_MARKETING_COLOR             =   'marketing_color';
    const CHARACTER_TYPE_SIZE                        =   'size';

	/*
     * @var ElasticExportHelper
     */
    private $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
    private $arrayHelper;

	/**
	 * AttributeValueNameRepositoryContract $AttributeValueNameRepository
	 */
    private $attributeValueNameRepository;

	/**
	 * PropertySelectionRepositoryContract $propertySelectionRepository
	 */
    private $propertySelectionRepository;

	/**
	 * @var array
	 */
    private $itemPropertyCache = [];

    /**
     * Geizhals constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     * @param AttributeValueNameRepositoryContract $attributeValueNameRepository
     * @param PropertySelectionRepositoryContract $propertySelectionRepository
     */
    public function __construct(
        ElasticExportHelper $elasticExportHelper,
        ArrayHelper $arrayHelper,
        AttributeValueNameRepositoryContract $attributeValueNameRepository,
        PropertySelectionRepositoryContract $propertySelectionRepository
    )
    {
        $this->elasticExportHelper = $elasticExportHelper;
        $this->arrayHelper = $arrayHelper;
        $this->attributeValueNameRepository = $attributeValueNameRepository;
        $this->propertySelectionRepository = $propertySelectionRepository;
    }
    
    /**
     * @param $resultData
     * @param array $formatSettings
     */
    protected function generateContent($resultData, array $formatSettings = [])
	{
		if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(";");

			$this->addCSVContent([
                // Required data for variations - but important for the sellers, should come first
                'Sku parent',

                // Mandatory data
                'Your reference',
                'EAN',
                'Brand',
                'Nature of product',
                'Category code',
                'Basket short wording',
                'Basket long wording',
                'Product description',
                'Picture 1 (jpeg)',

                // Required data for variations
                'Size',
                'Marketing color',

                // Optional data
                'Marketing description',
                'Picture 2 (jpeg)',
                'Picture 3 (jpeg)',
                'Picture 4 (jpeg)',
                'ISBN / GTIN',
                'MFPN',
                'Length ',
                'Width ',
                'Height ',
                'Weight ',


                // Specific data
                'Main color',
                'Gender',
                'Type of public',
                'Sports',
                'Comment'
			]);

            $rows = [];

			foreach($resultData as $item)
			{
                $variationAttributes = $this->getVariationAttributes($item, $settings);

                if(count($variationAttributes['color']) > 0)
                {
                    $color = $variationAttributes['color'];
                }
                elseif(strlen($this->getProperty($item, $settings, self::CHARACTER_TYPE_MARKETING_COLOR)))
                {
                    $color = $this->getProperty($item, $settings, self::CHARACTER_TYPE_MARKETING_COLOR);
                }
                else
                {
                    $color = '';
                }

                if(count($variationAttributes['size']) > 0)
                {
                    $size = $variationAttributes['size'];
                }
                elseif(strlen($this->getProperty($item, $settings, self::CHARACTER_TYPE_SIZE)))
                {
                    $size = $this->getProperty($item, $settings, self::CHARACTER_TYPE_SIZE);
                }
                else
                {
                    $size = '';
                }


                $lengthCm = $item->variationBase->lengthMm / 10;
                $widthCm = $item->variationBase->widthMm / 10;
                $heightCm = $item->variationBase->heightMm / 10;
                $weightKg = $item->variationBase->weightG / 1000;

				$data = [
					// Required data for variations - but important for the sellers, should come first
                    'Sku parent'                            =>  $item->itemBase->id,

                    // Mandatory data
                    'Your reference'                        =>  $item->variationMarketStatus->sku,
                    'EAN'                                   =>  $this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode')),
                    'Brand'                                 =>  $this->elasticExportHelper->getExternalManufacturerName($item->itemBase->producerId),
                    'Nature of product'                     =>  strlen($color) || strlen($size) ? 'variante' : 'standard',
                    'Category code'                         =>  $item->variationStandardCategory->categoryId,
                    'Basket short wording'                  =>  $this->elasticExportHelper->getName($item, $settings, 256),
                    'Basket long wording'                   =>  $item->itemDescription->shortDescription,
                    'Product description'                   =>  $this->getDescription($item, $settings),
                    'Picture 1 (jpeg)'                      =>  $this->getImageByNumber($item, $settings, 1),

                    // Required data for variations
                    'Size'                                  =>  $size,
                    'Marketing color'                       =>  $color,

                    // Optional data
                    'Marketing description'                 =>  $this->getProperty($item, $settings, self::CHARACTER_TYPE_MARKETING_DESCRIPTION),
                    'Picture 2 (jpeg)'                      =>  $this->getImageByNumber($item, $settings, 2),
                    'Picture 3 (jpeg)'                      =>  $this->getImageByNumber($item, $settings, 3),
                    'Picture 4 (jpeg)'                      =>  $this->getImageByNumber($item, $settings, 4),
                    'ISBN / GTIN'                           =>  $this->elasticExportHelper->getBarcodeByType($item, ElasticExportHelper::BARCODE_ISBN),
                    'MFPN'                                  =>  $item->variationBase->model,
                    'Length'                                =>  $lengthCm,
                    'Width'                                 =>  $widthCm,
                    'Height'                                =>  $heightCm,
                    'Weight'                                =>  $weightKg,



                    // Specific data
                    'Main color'                            =>  $this->getProperty($item, $settings, self::CHARACTER_TYPE_MAIN_COLOR),
                    'Gender'                                =>  $this->getProperty($item, $settings, self::CHARACTER_TYPE_GENDER),
                    'Type of public'                        =>  $this->getProperty($item, $settings, self::CHARACTER_TYPE_TYPE_OF_PUBLIC),
                    'Sports'                                =>  $this->getProperty($item, $settings, self::CHARACTER_TYPE_SPORTS),
                    'Comment'                               =>  $this->getProperty($item, $settings, self::CHARACTER_TYPE_COMMENT)
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

        $itemPropertyList = $this->getItemPropertyList($item);

        if(array_key_exists($property, $itemPropertyList))
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
            $characterMarketComponentList = $this->elasticExportHelper->getItemCharactersByComponent($item, 143.00);

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
    private function getVariationAttributes(Record $item, KeyValue $settings):array
    {
        $variationAttributes = [];

        foreach($item->variationAttributeValueList as $variationAttribute)
        {
            $attributeValueName = $this->attributeValueNameRepository->findOne($variationAttribute->attributeValueId, $settings->get('lang'));

            if($attributeValueName instanceof AttributeValueName)
            {
                if($attributeValueName->attributeValue->attribute->amazonAttribute)
                {
                    $variationAttributes[$attributeValueName->attributeValue->attribute->amazonAttribute][] = $attributeValueName->name;
                }
            }
        }

        return $variationAttributes;
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

<?php

namespace ElasticExport\ES_Generators;

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

class CdiscountCOM extends CSVGenerator
{
    const CHARACTER_TYPE_DESCRIPTION				    =   'description';
    const CHARACTER_TYPE_GENDER                         =   'gender';
    const CHARACTER_TYPE_TYPE_OF_PUBLIC                 =   'type_of_public';
    const CHARACTER_TYPE_SPORTS                         =   'sports';
    const CHARACTER_TYPE_WARNINGS                       =   'warnings';
    const CHARACTER_TYPE_COMMENT                        =   'comment';
    const CHARACTER_TYPE_MAIN_COLOR                     =   'main_color';
    const CHARACTER_TYPE_MARKETING_DESCRIPTION          =   'marketing_description';
    const CHARACTER_TYPE_MARKETING_COLOR                =   'marketing_color';
    const CHARACTER_TYPE_SIZE                           =   'size';

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
	 * PropertySelectionRepositoryContract $propertySelectionRepository
	 */
    private $propertySelectionRepository;

	/**
	 * @var array
	 */
    private $itemPropertyCache = [];

    /**
     * @var array $idlVariations
     */
    private $idlVariations = array();

    /**
     * CdiscountCOM constructor.
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
     * @param array $resultData
     * @param array $formatSettings
     */
    protected function generateContent($resultData, array $formatSettings = [])
	{
		if(is_array($resultData) && count($resultData['documents']) > 0)
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


            //Create a List of all VariationIds
            $variationIdList = array();
            foreach($resultData['documents'] as $variation)
            {
                $variationIdList[] = $variation['id'];
            }

            //Get the missing fields in ES from IDL
            if(is_array($variationIdList) && count($variationIdList) > 0)
            {
                /**
                 * @var \ElasticExport\ES_IDL_ResultList\CdiscountCOM $idlResultList
                 */
                $idlResultList = pluginApp(\ElasticExport\ES_IDL_ResultList\CdiscountCOM::class);
                $idlResultList = $idlResultList->getResultList($variationIdList, $settings);
            }

            //Creates an array with the variationId as key to surpass the sorting problem
            if(isset($idlResultList) && $idlResultList instanceof RecordList)
            {
                $this->createIdlArray($idlResultList);
            }

			foreach($resultData['documents'] as $item)
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


                $lengthCm = $item['data']['variation']['lengthMM'] / 10;
                $widthCm = $item['data']['variation']['widthMM'] / 10;
                $heightCm = $item['data']['variation']['heightMM'] / 10;
                $weightKg = $item['data']['variation']['weightG'] / 1000;

				$data = [
					// Required data for variations - but important for the sellers, should come first
                    'Sku parent'                            =>  $item['data']['item']['id'],

                    // Mandatory data
                    'Your reference'                        =>  $item['data']['skus']['sku'],
                    'EAN'                                   =>  $this->elasticExportHelper->getEsBarcodeByType($item, $settings->get('barcode')),
                    'Brand'                                 =>  $this->elasticExportHelper->getExternalManufacturerName((int)$item['data']['item']['manufacturer']['id']),
                    'Nature of product'                     =>  strlen($color) || strlen($size) ? 'variante' : 'standard',
                    'Category code'                         =>  $item['data']['defaultCategories']['id'],
                    'Basket short wording'                  =>  $this->elasticExportHelper->getEsName($item, $settings, 256),
                    'Basket long wording'                   =>  $item['data']['texts']['shortDescription']->itemDescription->shortDescription,
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
                    'ISBN / GTIN'                           =>  $this->elasticExportHelper->getEsBarcodeByType($item, ElasticExportHelper::BARCODE_ISBN),
                    'MFPN'                                  =>  $item['data']['variation']['model'],
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
     * @param  array   $item
     * @param  KeyValue $settings
     * @param  string   $property
     * @return string
     */
    private function getProperty($item, KeyValue $settings, string $property):string
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
     * @param 	array $item
     * @return array<string,string>
     */
    private function getItemPropertyList($item):array
    {
        if(!array_key_exists($item['data']['item']['id'], $this->itemPropertyCache))
        {
            $characterMarketComponentList = $this->elasticExportHelper->getEsItemCharactersByComponent($this->idlVariations[$item['id']], 143.00);

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

            $this->itemPropertyCache[$item['data']['item']['id']] = $list;
        }

        return $this->itemPropertyCache[$item['data']['item']['id']];
    }

    /**
     * Get item description.
     * @param array $item
     * @param KeyValue $settings
     * @return string
     */
    private function getDescription($item, KeyValue $settings):string
    {
        $description = $this->elasticExportHelper->getEsItemCharacterByBackendName($this->idlVariations[$item['id']], $settings, self::CHARACTER_TYPE_DESCRIPTION);

        if (strlen($description) <= 0)
        {
            $description = $this->elasticExportHelper->getEsDescription($item, $settings, 5000);
        }

        return $description;
    }

    /**
     * Get variation attributes.
     * @param  array   $item
     * @param  KeyValue $settings
     * @return array<string,string>
     */
    private function getVariationAttributes($item, KeyValue $settings):array
    {
        $variationAttributes = [];

        foreach($item['data']['attributes'] as $variationAttribute)
        {
            $attributeValueName = $this->attributeValueNameRepository->findOne($variationAttribute['valueId'], $settings->get('lang'));

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
	 * @param array $item
	 * @param KeyValue $settings
	 * @param int $number
	 * @return string
	 */
    private function getImageByNumber($item, KeyValue $settings, int $number):string
    {
        $imageList = $this->elasticExportHelper->getEsImageList($item, $settings);

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
     * @param RecordList $idlResultList
     */
    private function createIdlArray($idlResultList)
    {
        if($idlResultList instanceof RecordList)
        {
            foreach($idlResultList as $idlVariation)
            {
                if($idlVariation instanceof Record)
                {
                    $this->idlVariations[$idlVariation->variationBase->id] = [
                        'itemBase.id' => $idlVariation->itemBase->id,
                        'variationBase.id' => $idlVariation->variationBase->id,
                        'itemPropertyList' => $idlVariation->itemPropertyList,
                        'variationRetailPrice.price' => $idlVariation->variationRetailPrice->price,
                    ];
                }
            }
        }
    }
}

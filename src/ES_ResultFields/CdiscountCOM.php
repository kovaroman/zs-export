<?php

namespace ElasticExport\ES_ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\Search\Mutators\ImageMutator;
use Plenty\Modules\Cloud\ElasticSearch\Lib\Source\Mutator\BuiltIn\LanguageMutator;
use Plenty\Modules\Item\Search\Mutators\SkuMutator;

class CdiscountCOM extends ResultFields
{
    /*
     * @var ArrayHelper
     */
	private $arrayHelper;

    /**
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ArrayHelper $arrayHelper)
    {
        $this->arrayHelper = $arrayHelper;
    }

    public function generateResultFields(array $formatSettings = []):array
    {
        $settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

        $reference = $settings->get('referrerId') ? $settings->get('referrerId') : 143;

        $itemDescriptionFields = ['texts.urlPath'];

        switch($settings->get('nameId'))
        {
            case 1:
                $itemDescriptionFields[] = 'texts.name1';
                break;
            case 2:
                $itemDescriptionFields[] = 'texts.name2';
                break;
            case 3:
                $itemDescriptionFields[] = 'texts.name3';
                break;
            default:
                $itemDescriptionFields[] = 'texts.name1';
                break;
        }

        $itemDescriptionFields = [
            'texts.shortDescription',
            'texts.description',
            'texts.technicalData'];

        /**
         * @var ImageMutator $imageMutator
         */
        $imageMutator = pluginApp(ImageMutator::class);
        $imageMutator->addMarket($reference);
        /**
         * @var LanguageMutator $languageMutator
         */
        $languageMutator = pluginApp(LanguageMutator::class, [[$settings->get('lang')]]);
        /**
         * @var SkuMutator $skuMutator
         */
        $skuMutator = pluginApp(SkuMutator::class);
        $skuMutator->setMarket($reference);

        $fields = [
            [
                //item
                'item.id',
                'item.manufacturer.id',

                //variation
                'id',
                'variation.model',
                'variation.lengthMM',
                'variation.heightMM',
                'variation.weightG',
                'variation.widthMM',

                //images
                'images.item.type',
                'images.item.path',
                'images.item.position',
                'images.variation.type',
                'images.variation.path',
                'images.variation.position',

                //unit
                'unit.content',
                'unit.id',

                //sku
                'skus.sku',

                //defaultCategories
                'defaultCategories.id',

                //barcodes
                'barcodes.code',
                'barcodes.id',
                'barcodes.type',

                //attributes
                'attributes.valueId',

            ],

            [
                $imageMutator,
                $languageMutator,
                $skuMutator
            ],
        ];
        foreach($itemDescriptionFields as $itemDescriptionField)
        {
            $fields[0][] = $itemDescriptionField;
        }

        return $fields;
//        $itemDescriptionFields = [
//            'urlContent',         done
//            'shortDescription',   done
//            'description',        done
//            'technicalData'       done
//        ];
//
//        $itemDescriptionFields[] = ($settings->get('nameId')) ? 'name' . $settings->get('nameId') : 'name1';      done
//
//        $fields =
//            [
//                'itemBase'  =>  [
//                    'id',             done
//                    'producerId',     done
//                ],
//
//                'itemDescription'   =>  [
//                    'params' => [
//                        'language' => $settings->get('lang') ? $settings->get('lang') : 'fr',
//                    ],
//                    'fields' => $itemDescriptionFields,   done
//                ],
//
//                'itemPropertyList' => [
//    				'params' => [],
//    				'fields' => [
//    					'itemPropertyId',           //todo grab from idl
//    					'propertyId',               //todo grab from idl
//    					'propertyValue',            //todo grab from idl
//    					'propertyValueType',        //todo grab from idl
//    					'isOrderProperty',          //todo grab from idl
//    					'propertyOrderMarkup'       //todo grab from idl
//    				]
//    			],
//
//                'variationBase' =>  [
//                    'id',                         //done
//                    'model'                       //done
//                ],
//
//                'variationRetailPrice' => [
//					'params' => [
//						'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : 143,
//					],
//					'fields' => [
//						'price',                //todo grab from idl
//					],
//                ],
//
//                'variationImageList' => [
//                    'params' => [
//                        'type' => 'item_variation',
//                        'referenceMarketplace' => $settings->get('referrerId') ? $settings->get('referrerId') : 143,
//                    ],
//                    'fields' => [
//                        'type',               //done
//                        'path',               //done
//                        'position',           //done
//                    ]
//                ],
//
//                'variationBarcodeList' => [
//                    'params' => [
//                        'barcodeType' => $settings->get('barcode') ? $settings->get('barcode') : 'EAN',
//                    ],
//                    'fields' => [
//                        'code',           //done
//                        'barcodeId',      //done
//                    ]
//                ],
//
//                'variationStandardCategory' =>  [
//                    'params'    =>  [
//                        'plentyId'  =>  $settings->get('plentyId')
//                    ],
//                    'fields'    =>  [
//                        'categoryId',     //done
//                    ]
//                ],
//
//                'variationMarketStatus' => [
//                    'params' => [
//                        'marketId' => 143,
//                    ],
//                    'fields' => [
//                        'sku'         //done
//                    ]
//                ],
//
//                'variationAttributeValueList' => [
//				    'attributeId',          //done
//				    'attributeValueId'      //done
//			    ],
//            ];
//
//        return $fields;
    }
}

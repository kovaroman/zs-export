<?php

namespace ElasticExport\ES_ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\Search\Mutators\ImageMutator;
use Plenty\Modules\Cloud\ElasticSearch\Lib\Source\Mutator\BuiltIn\LanguageMutator;
use Plenty\Modules\Item\Search\Mutators\SkuMutator;
/**
 * Class BilligerDE
 * @package ElasticExport\ResultFields
 */
class BilligerDE extends ResultFields
{
    /*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

    /**
     * Billiger constructor.
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ArrayHelper $arrayHelper)
    {
		$this->arrayHelper = $arrayHelper;
    }

    /**
     * Generate result fields.
     * @param  array $formatSettings = []
     * @return array
     */
    public function generateResultFields(array $formatSettings = []):array
    {
        $settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

        $reference = $settings->get('referrerId') ? $settings->get('referrerId') : -1;

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

        if($settings->get('descriptionType') == 'itemShortDescription'
            || $settings->get('previewTextType') == 'itemShortDescription')
        {
            $itemDescriptionFields[] = 'texts.shortDescription';
        }

        if($settings->get('descriptionType') == 'itemDescription'
            || $settings->get('descriptionType') == 'itemDescriptionAndTechnicalData'
            || $settings->get('previewTextType') == 'itemDescription'
            || $settings->get('previewTextType') == 'itemDescriptionAndTechnicalData')
        {
            $itemDescriptionFields[] = 'texts.description';
        }
        $itemDescriptionFields[] = 'texts.technicalData';

        //Mutator
        /**
         * @var ImageMutator $imageMutator
         */
        $imageMutator = pluginApp(ImageMutator::class);
        $imageMutator->addMarket($reference);
        /**
         * @var LanguageMutator $languageMutator
         */
        $languageMutator = pluginApp(LanguageMutator::class, [[$settings->get('lang')]]);

        $fields = [
            [
                //item
                'item.id',
                'item.manufacturer.id',

                //variation
                'id',
                'variation.availability.id',
                'variation.stockLimitation',
                'variation.vatId',
                'variation.model',
                'variation.isMain',

                //images todo ask if all is needed or maybe variation + all in case of no variation images
                'images.item.type',
                'images.item.path',
                'images.item.position',
                'images.item.fileType',
                'images.variation.type',
                'images.variation.path',
                'images.variation.position',
                'images.variation.fileType',

                //unit
                'unit.content',
                'unit.id',

                //defaultCategories
                'defaultCategories.id',

                //barcodes
                'barcodes.code',
                'barcodes.type',

                //attributes
                'attributes.attributeValueSetId',
                'attributes.attributeId',
                'attributes.valueId',

            ],

            [
                $imageMutator,
                $languageMutator
            ],
        ];
        foreach($itemDescriptionFields as $itemDescriptionField)
        {
            $fields[0][] = $itemDescriptionField;
        }

        return $fields;
//
//        return [
//            'itemBase'=> [
//                'id',                 done
//                'producerId',         done
//                'variationCount',     not needed with grouper //todo write a grouper like in rakutende and write a new method in the generator which fill the datafields
//            ],
//
//            'itemDescription' => [
//                'params' => [
//                    'language' => $settings->get('lang') ? $settings->get('lang') : 'de',
//                ],
//                'fields' => $itemDescriptionFields,       done
//            ],
//
//            'variationImageList' => [
//                'params' => [
//                    'type' => 'item_variation',
//                    'referenceMarketplace' => $settings->get('referrerId') ? $settings->get('referrerId') : -1,
//                ],
//                'fields' => [
//                    'type',       done
//                    'path',       done
//                    'position',   done
//                ]
//
//            ],
//
//            'variationBase' => [
//                'id',                         done
//                'availability',               done
//                'attributeValueSetId',        done
//                'model',                      done
//                'limitOrderByStockSelect',    done
//                'unitId',                     done
//                'content',                    done
//            ],
//
//            'variationRetailPrice' => [
//				'params' => [
//					'referrerId' => $settings->get('referrerId'),
//				],
//				'fields' => [
//					'price',        //todo grab from idl
//				],
//            ],
//
//            'variationStandardCategory' => [
//                'params' => [
//                    'plentyId' => $settings->get('plentyId'),
//                ],
//                'fields' => [
//                    'categoryId',     done
//                    'plentyId',       not needed
//                    'manually',       not needed
//                ],
//            ],
//
//            'variationBarcodeList' => [
//                'params' => [
//                    'barcodeType' => $settings->get('barcode') ? $settings->get('barcode') : 'EAN',
//                ],
//                'fields' => [
//					'variationId',      not needed
//                	'code',             done
//                    'barcodeId',      not needed
//					'barcodeType',      done
//					'barcodeName',      not needed
//                ]
//            ],
//			'variationAttributeValueList' => [
//				'attributeId',          done
//				'attributeValueId',     done
//			],
//        ];
    }
}

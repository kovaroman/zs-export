<?php

namespace ElasticExport\ES_ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\Search\Mutators\ImageMutator;
use Plenty\Modules\Cloud\ElasticSearch\Lib\Source\Mutator\BuiltIn\LanguageMutator;
/**
 * Class BasicPriceSearchEngine
 * @package ElasticExport\ResultFields
 */
class BasicPriceSearchEngine extends ResultFields
{
    /*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

    /**
     * BasicPriceSearchEngine constructor.
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
                'item.amazonFedas',

                //variation
                'id',
                'variation.availability.id',
                'variation.model',
                'variation.weightG',

                //texts
                'texts.name'.$settings->get('nameId'),
                'texts.shortDescription',
                'texts.description',
                'texts.technicalData',
                'texts.urlPath',

                //unit
                'unit.content',
                'unit.id',

                //images
                'images.item.id',
                'images.item.type',
                'images.item.fileType',
                'images.item.path',
                'images.item.position',
                'images.item.cleanImageName',
                'images.variation.id',
                'images.variation.type',
                'images.variation.fileType',
                'images.variation.path',
                'images.variation.position',
                'images.variation.cleanImageName',

                //defaultCategories
                'defaultCategories.id',

                //barcodes
                'barcodes.code',
                'barcodes.type',
                'barcodes.id',
                'barcodes.name',
            ],

            [
                $imageMutator,
                $languageMutator,
            ],
        ];

        return $fields;

//        return [
//            'itemBase'=> [
//                'id',                 done
//                'producerId',         done
//                'fedas'               done
//            ],
//
//            'itemDescription' => [
//                'params' => [
//                    'language' => $settings->get('lang') ? $settings->get('lang') : 'de',
//                ],
//                'fields' => [
//					$settings->get('nameId') ? 'name' . $settings->get('nameId') : 'name1',     done
//					'description',                  done
//					'shortDescription',             done
//					'technicalData',                done
//                ],
//            ],
//
//            'variationImageList' => [
//                'params' => [
//                    'type' => 'item_variation',
//                    'referenceMarketplace' => $settings->get('referrerId'),
//                ],
//                'fields' => [
//                    'imageId',        done
//                    'type',           done
//                    'fileType',       done
//                    'path',           done
//                    'position',       done
//                    'cleanImageName', done
//                ]
//
//            ],
//
//            'variationBase' => [
//                'availability',       done
//                'model',              done
//                'customNumber',       //todo grab from idl
//                'weightG'             done
//            ],
//
//            'variationRetailPrice' => [
//				'params' => [
//					'referrerId' => $settings->get('referrerId'),
//				],
//				'fields' => [
//					'price',
//                    'retailPrice',        //todo grab from idl
//                    'retailPriceNet',     //todo grab from idl
//                    'basePrice',          //todo grab from idl
//                    'basePriceNet',       //todo grab from idl
//                    'unitPrice',          //todo grab from idl
//                    'unitPriceNet',       //todo grab from idl
//                ],
//            ],
//
//            'variationRecommendedRetailPrice' => [
//				'params' => [
//					'referrerId' => $settings->get('referrerId'),
//				],
//				'fields' => [
//					'price',                //todo grab from idl
//				],
//            ],
//
//            'variationStandardCategory' => [
//                'params' => [
//                    'plentyId' => $settings->get('plentyId'),
//                ],
//                'fields' => [
//                    'categoryId'      done
//                ],
//            ],
//
//            'variationBarcodeList' => [
//                'params' => [
//                    'barcodeType' => $settings->get('barcode'),
//                ],
//                'fields' => [
//                    'code',           done
//                    'barcodeId',      done
//					'barcodeType',      done
//					'barcodeName',      done
//					'variationId'
//                ]
//            ],
//        ];
    }
}

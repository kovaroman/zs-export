<?php

namespace ElasticExport\ES_ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\Search\Mutators\ImageMutator;
use Plenty\Modules\Cloud\ElasticSearch\Lib\Source\Mutator\BuiltIn\LanguageMutator;
use Plenty\Modules\Item\Search\Mutators\SkuMutator;
/**
 * Class RakutenDE
 * @package ElasticExport\ResultFields
 */
class RakutenDE extends ResultFields
{
	/*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

	/**
	 * Rakuten constructor.
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

        $this->setOrderByList(['variation.itemId', 'ASC']);

        $reference = $settings->get('referrerId') ? $settings->get('referrerId') : 106;

        $itemDescriptionFields = ['urlContent'];
        $itemDescriptionFields[] = ($settings->get('nameId')) ? 'name' . $settings->get('nameId') : 'name1';

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
                'item.rakutenCategoryId',

                //variation
                'id',
                'variation.availability.id',
                'variation.stockLimitation',
                'variation.vatId',
                'variation.model',
                'variation.isMain',

                //text
                implode(', ', $itemDescriptionFields),

                //images
                'images.all.type',
                'images.all.path',
                'images.all.position',

                //unit
                'unit.content',
                'unit.id',

                //sku
                'skus.sku',

                //defaultCategories
                'defaultCategories.id',

                //barcodes
                'barcodes.code',
                'barcodes.type',

                //attributes
                'attributes.attributeId',
                'attributes.valueId',
            ],

            [
                $imageMutator,
                $languageMutator
            ],
//			'itemBase'=> [
//				'id', done
//				'producerId', done
//				'free1',   doesnt exist in elastic search   todo grab from idl
//				'free2',   doesnt exist in elastic search   todo grab from idl
//				'free3',   doesnt exist in elastic search   todo grab from idl
//				'free4',   doesnt exist in elastic search   todo grab from idl
//				'free5',   doesnt exist in elastic search   todo grab from idl
//				'free6',   doesnt exist in elastic search   todo grab from idl
//				'free7',   doesnt exist in elastic search   todo grab from idl
//				'free8',   doesnt exist in elastic search   todo grab from idl
//				'free9',   doesnt exist in elastic search   todo grab from idl
//				'free10',  doesnt exist in elastic search   todo grab from idl
//				'free11',  doesnt exist in elastic search   todo grab from idl
//				'free12',  doesnt exist in elastic search   todo grab from idl
//				'free13',  doesnt exist in elastic search   todo grab from idl
//				'free14',  doesnt exist in elastic search   todo grab from idl
//				'free15',  doesnt exist in elastic search   todo grab from idl
//				'free16',  doesnt exist in elastic search   todo grab from idl
//				'free17',  doesnt exist in elastic search   todo grab from idl
//				'free18',  doesnt exist in elastic search   todo grab from idl
//				'free19',  doesnt exist in elastic search   todo grab from idl
//				'free20',  doesnt exist in elastic search   todo grab from idl
//				'tradoriaCategory'  done
//			],
//
//			'itemDescription' => [
//                'params' => [
//                    'language' => $settings->get('lang') ? $settings->get('lang') : 'de',
//                ],
//                'fields' => $itemDescriptionFields, done
//            ],
//
//			'itemPropertyList' => [
//				'params' => [],
//				'fields' => [
//					'propertyId',       //research
//					'propertyValue',    //research
//				]
//			],
//
//			'variationImageList' => [
//				'params' => [
//					'type' => 'all',
//                    'referenceMarketplace' => $settings->get('referrerId') ? $settings->get('referrerId') : 106,
//				],
//				'fields' => [
//					'type',     done
//					'path',     done
//					'position', done
//				]
//			],
//
//			'variationBase' => [
//				'availability',             done
//				'attributeValueSetId',      done
//				'content',                  done
//				'id',                       done
//				'limitOrderByStockSelect',  done
//				'model',                    done
//				'unitId',                   done
//				'vatId',                    done
//              'primaryVariation',         done
//			],
//
//			'variationRecommendedRetailPrice' => [
//				'params' => [
//					'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : 106,
//				],
//				'fields' => [
//					'price',    //todo grab from idl
//				],
//			],
//
//            'variationRetailPrice' => [
//				'params' => [
//					'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : 106,
//				],
//				'fields' => [
//					'price',    //todo grab from idl
//				],
//            ],
//
//			'variationSpecialOfferRetailPrice' => [
//				'params' => [
//					'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : 106,
//				],
//				'fields' => [
//					'retailPrice',  //todo grab from idl
//				],
//			],
//
//			'variationStandardCategory' => [
//				'params' => [
//					'plentyId' => $settings->get('plentyId'),
//				],
//				'fields' => [
//					'categoryId',   done
//				],
//			],
//
//			'variationBarcodeList' => [
//				'params' => [
//					'barcodeType' => $settings->get('barcode') ? $settings->get('barcode') : 'EAN',
//				],
//				'fields' => [
//					'code',             done
//					'barcodeType',      done
//				]
//			],
//
//			'variationMarketStatus' => [
//				'params' => [
//					'marketId' => 106
//				],
//				'fields' => [
//					'sku'       //done
//				]
//			],
//
//			'variationStock' => [
//				'params' => [
//					'type' => 'virtual'
//				],
//				'fields' => [
//					'stockNet'   //todo grab from idl
//				]
//			],
//
//			'variationAttributeValueList' => [
//				'attributeId',      //done
//				'attributeValueId'  //done
//			]

		];


		return $fields;
	}
}

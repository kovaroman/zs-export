<?php

namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class RakutenDE
 * @package ElasticExport\ResultFields
 */
class RakutenDE extends ResultFields
{
	const MARKET_REFERENCE_RAKUTEN_DE = 106.00;

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

        $this->setOrderByList(['orderBy.itemId' => 'asc']);

        $itemDescriptionFields = ['urlContent'];
        $itemDescriptionFields[] = ($settings->get('nameId')) ? 'name' . $settings->get('nameId') : 'name1';

		if($settings->get('descriptionType') == 'itemShortDescription'
            || $settings->get('previewTextType') == 'itemShortDescription')
        {
            $itemDescriptionFields[] = 'shortDescription';
        }

        if($settings->get('descriptionType') == 'itemDescription'
            || $settings->get('descriptionType') == 'itemDescriptionAndTechnicalData'
            || $settings->get('previewTextType') == 'itemDescription'
            || $settings->get('previewTextType') == 'itemDescriptionAndTechnicalData')
        {
            $itemDescriptionFields[] = 'description';
        }

        $itemDescriptionFields[] = 'technicalData';

		$fields = [
			'itemBase'=> [
				'id',
				'producerId',
				'free1',
				'free2',
				'free3',
				'free4',
				'free5',
				'free6',
				'free7',
				'free8',
				'free9',
				'free10',
				'free11',
				'free12',
				'free13',
				'free14',
				'free15',
				'free16',
				'free17',
				'free18',
				'free19',
				'free20',
				'tradoriaCategory'
			],

			'itemDescription' => [
                'params' => [
                    'language' => $settings->get('lang') ? $settings->get('lang') : 'de',
                ],
                'fields' => $itemDescriptionFields,
            ],

			'itemPropertyList' => [
				'params' => [],
				'fields' => [
					'propertyId',
					'propertyValue',
				]
			],

			'variationImageList' => [
				'params' => [
					'type' => 'all',
                    'referenceMarketplace' => $settings->get('referrerId') ? $settings->get('referrerId') : self::MARKET_REFERENCE_RAKUTEN_DE,
				],
				'fields' => [
					'type',
					'path',
					'position',
				]
			],

			'variationBase' => [
				'availability',
				'attributeValueSetId',
				'content',
				'id',
				'limitOrderByStockSelect',
				'model',
				'unitId',
				'vatId',
                'primaryVariation',
			],

			'variationRecommendedRetailPrice' => [
				'params' => [
					'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : self::MARKET_REFERENCE_RAKUTEN_DE,
				],
				'fields' => [
					'price',
				],
			],

            'variationRetailPrice' => [
				'params' => [
					'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : self::MARKET_REFERENCE_RAKUTEN_DE,
				],
				'fields' => [
					'price',
				],
            ],

			'variationSpecialOfferRetailPrice' => [
				'params' => [
					'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : self::MARKET_REFERENCE_RAKUTEN_DE,
				],
				'fields' => [
					'retailPrice',
				],
			],

			'variationStandardCategory' => [
				'params' => [
					'plentyId' => $settings->get('plentyId'),
				],
				'fields' => [
					'categoryId',
				],
			],

			'variationBarcodeList' => [
				'params' => [
					'barcodeType' => $settings->get('barcode') ? $settings->get('barcode') : 'EAN',
				],
				'fields' => [
					'code',
					'barcodeType',
				]
			],

			'variationMarketStatus' => [
				'params' => [
					'marketId' => self::MARKET_REFERENCE_RAKUTEN_DE
				],
				'fields' => [
					'sku'
				]
			],

			'variationStock' => [
				'params' => [
					'type' => 'virtual'
				],
				'fields' => [
					'stockNet'
				]
			],

			'variationAttributeValueList' => [
				'attributeId',
				'attributeValueId'
			]
		];


		return $fields;
	}
}

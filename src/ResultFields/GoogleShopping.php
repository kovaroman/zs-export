<?php

namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class GoogleShopping
 * @package ElasticExport\ResultFields
 */
class GoogleShopping extends ResultFields
{
	/*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

	/**
	 * GoogleShopping constructor.
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

        if($settings->get('descriptionType') == 'technicalData'
            || $settings->get('descriptionType') == 'itemDescriptionAndTechnicalData'
            || $settings->get('previewTextType') == 'technicalData'
            || $settings->get('previewTextType') == 'itemDescriptionAndTechnicalData')
        {
            $itemDescriptionFields[] = 'technicalData';
        }

		$fields = [
			'itemBase'=> [
				'id',
				'producerId',
				'apiCondition',
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
					'itemPropertyId',
					'propertyId',
					'propertyValue',
					'propertyValueType',
					'isOrderProperty',
					'propertyOrderMarkup'
				]
			],

            'variationImageList' => [
                'params' => [
                    'itemImages' => [
                        'type'                 => 'item', // all images
                        'imageType'            => ['internal'],
                        'referenceMarketplace' => $settings->get('referrerId') ? $settings->get('referrerId') : 7,
                    ],
                    'variationImages'                    => [
                        'type'                 => 'variation', // current variation images
                        'imageType'            => ['internal'],
                        'referenceMarketplace' => $settings->get('referrerId') ? $settings->get('referrerId') : 7,
                    ],
                ],
                'fields' => [
                    'type',
                    'path',
                    'position',
                ],
            ],

			'variationBase' => [
				'id',
				'availability',
				'attributeValueSetId',
				'model',
				'limitOrderByStockSelect',
				'unitId',
				'content',
				'weightG',
			],

			'variationSpecialOfferRetailPrice' => [
				'params' => [
					'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : 7,
				],
				'fields' => [
					'retailPriceId',
					'retailPrice',
					'retailPriceNet',
					'basePrice',
					'basePriceNet',
					'unitPrice',
					'unitPriceNet',
					'orderParamsMarkup',
					'orderParamsMarkupNet',
					'vatId',
					'vatValue',
					'currency',
					'exchangeRatio',
					'lastUpdateTimestamp'
				],
			],
			'variationRetailPrice' => [
				'params' => [
					'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : 7,
				],
				'fields' => [
					'price',
				],
			],

			'variationStandardCategory' => [
				'params' => [
					'plentyId' => $settings->get('plentyId'),
				],
				'fields' => [
					'categoryId',
					'plentyId',
					'manually',
				],
			],

			'variationBarcodeList' => [
				'params' => [
					'barcodeType' => $settings->get('barcode') ? $settings->get('barcode') : 'EAN',
				],
				'fields' => [
					'code',
					'barcodeId',
				]
			],

			'variationAttributeValueList' => [
				'attributeId',
				'attributeValueId',
			],
		];


		return $fields;
	}
}

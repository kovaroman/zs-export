<?php

namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class BelboonDE
 * @package ElasticExport\ResultFields
 */
class BelboonDE extends ResultFields
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

        $itemDescriptionFields = [
            'keywords',
            ($settings->get('nameId') ? 'name' . $settings->get('nameId') : 'name1'),
            'urlContent',
        ];

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

		return [
            'itemBase'=> [
                'id',
                'producer',
            ],

            'itemDescription' => [
                'params' => [
                    'language' => $settings->get('lang') ? $settings->get('lang') : 'de',
                ],
                'fields' => $itemDescriptionFields,
            ],

            'variationImageList' => [
                'params' => [
                    'type' => 'item_variation',
                    'referenceMarketplace' => $settings->get('referrerId'),
                ],
                'fields' => [
                    'imageId',
                    'type',
                    'fileType',
                    'path',
                    'position',
                    'cleanImageName',
                ]
            ],

			'variationBarcode' => [
				'params' => [
					'barcodeType' => $settings->get('barcode') ? $settings->get('barcode') : 'EAN',
				],
				'fields' => [
					'code',
					'barcodeId',
				]
			],

            'variationBase' => [
				'attributeValueSetId',
				'availability',
				'content',
				'customNumber',
				'id',
				'limitOrderByStockSelect',
                'model',
				'unitId',
				'vatId'
            ],

            'variationRetailPrice' => [
				'params' => [
					'referrerId' => $settings->get('referrerId'),
				],
				'fields' => [
					'currency',
					'price',
				],
            ],

            'variationStandardCategory' => [
                'params' => [
                    'plentyId' => $settings->get('plentyId'),
                ],
                'fields' => [
                    'categoryId'
                ],
            ],

			'variationStock' => [
				'params' => [
					'type' => 'virtual',
				],
				'fields' => [
					'stockNet',
				]
			],
        ];
    }
}

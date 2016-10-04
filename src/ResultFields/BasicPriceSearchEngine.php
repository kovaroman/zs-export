<?php

namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

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

        return [
            'itemBase'=> [
                'id',
                'producer',
                'fedas'
            ],

            'itemDescription' => [
                'params' => [
                    'language' => $settings->get('lang') ? $settings->get('lang') : 'de',
                ],
                'fields' => [
					$settings->get('nameId') ? 'name' . $settings->get('nameId') : 'name1',
					'description',
					'shortDescription',
					'technicalData',
                ],
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

            'variationBase' => [
                'availability',
                'model',
                'customNumber',
                'weightG'
            ],

            'variationRetailPrice' => [
				'params' => [
					'referrerId' => $settings->get('referrerId'),
				],
				'fields' => [
					'price',
                    'retailPrice',
                    'retailPriceNet',
                    'basePrice',
                    'basePriceNet',
                    'unitPrice',
                    'unitPriceNet',
                ],
            ],

            'variationRecommendedRetailPrice' => [
				'params' => [
					'referrerId' => $settings->get('referrerId'),
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
                    'categoryId'
                ],
            ],

            'variationBarcodeList' => [
                'params' => [
                    'barcodeType' => $settings->get('barcode'),
                ],
                'fields' => [
                    'code',
                    'barcodeId',
					'barcodeType',
					'barcodeName',
					'variationId'
                ]
            ],
        ];
    }
}

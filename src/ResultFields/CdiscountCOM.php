<?php

namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

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

        $itemDescriptionFields = [
            'urlContent',
            'shortDescription',
            'description',
            'technicalData'
        ];

        $itemDescriptionFields[] = ($settings->get('nameId')) ? 'name' . $settings->get('nameId') : 'name1';

        return
            [
                'itemBase'  =>  [
                    'id',
                    'producer',
                ],

                'itemDescription'   =>  [
                    'params' => [
                        'language' => $settings->get('lang') ? $settings->get('lang') : 'fr',
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

                'variationBase' =>  [
                    'id',
                    'model'
                ],

                'variationRetailPrice' => [
					'params' => [
						'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : 143,
					],
					'fields' => [
						'price',
					],
                ],

                'variationImageList' => [
                    'params' => [
                        'type' => 'item_variation',
                        'referenceMarketplace' => $settings->get('referrerId') ? $settings->get('referrerId') : 143,
                    ],
                    'fields' => [
                        'type',
                        'path',
                        'position',
                    ]
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

                'variationStandardCategory' =>  [
                    'params'    =>  [
                        'plentyId'  =>  $settings->get('plentyId')
                    ],
                    'fields'    =>  [
                        'categoryId',
                    ]
                ],

                'variationMarketStatus' => [
                    'params' => [
                        'marketId' => 143,
                    ],
                    'fields' => [
                        'sku'
                    ]
                ],

                'variationAttributeValueList' => [
				    'attributeId',
				    'attributeValueId'
			    ],
            ];
    }
}

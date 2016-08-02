<?hh //strict

namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

class CdiscountCOM extends ResultFields
{
    /*
     * @var ArrayHelper
     */
private ArrayHelper $arrayHelper;

    /**
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ArrayHelper $arrayHelper)
    {
        $this->arrayHelper = $arrayHelper;
    }

    public function generateResultFields(array<FormatSetting> $formatSettings = []):array<string, mixed>
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

                'variationBase' =>  [
                    'id',
                    'model'
                ],

                'variationRetailPrice' => [
                    'price',
                ],

                'variationImageList' => [
                    'params' => [
                        'type' => 'variation',
                        'referenceMarketplace' => 143,
                    ],
                    'fields' => [
                        'type',
                        'path',
                        'position',
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

                'variationStandardCategory' =>  [
                    'params'    =>  [
                        'plentyId'  =>  $settings->get('plentyId') ? $settings->get('plentyId') : 1000
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
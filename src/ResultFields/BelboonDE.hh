<?hh //strict
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
	private ArrayHelper $arrayHelper;

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
     * @param  array<FormatSetting> $formatSettings = []
     * @return array
     */
    public function generateResultFields(array<FormatSetting> $formatSettings = []):array<string, mixed>
    {
        $settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

		if($settings->get('variations') == 'mainVariations')
        {
            $this->setGroupByList(['groupBy.itemIdGetPrimaryVariation']);
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
                'fields' => [
                    'description',
                    'keywords',
                    ($settings->get('nameId') ? 'name' . $settings->get('nameId') : 'name1'),
                    'shortDescription',
                    'technicalData',
                    'urlContent',
        		]
            ],

            'variationImageList' => [
                'params' => [
                    'type' => 'variation',
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
                'currency',
                'price',
            ],

            'variationStandardCategory' => [
                'params' => [
                    'plentyId' => $settings->get('plentyId') ? $settings->get('plentyId') : 1000,
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

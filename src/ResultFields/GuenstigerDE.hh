<?hh //strict
namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class GuenstigerDE
 * @package ElasticExport\ResultFields
 */
class GuenstigerDE extends ResultFields
{
    /*
	 * @var ArrayHelper
	 */
	private ArrayHelper $arrayHelper;

    /**
     * Guenstiger constructor.
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
					'name1',
					'name2',
					'name3',
					'description',
					'shortDescription',
					'technicalData',
                ],
            ],

            'variationImageList' => [
                'params' => [
                    'type' => 'item_variation',
                    'referenceMarketplace' => $settings->get('referrerId') ? $settings->get('referrerId') : -1,
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
            ],

            'variationRetailPrice' => [
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

            'variationBarcodeList' => [
                'params' => [
                    'barcodeType' => $settings->get('barcode'),
                ],
                'fields' => [
                    'code',
                    'barcodeId',
                ]
            ],

            'variationBarcodeList' => [
                'variationId',
                'code',
                'barcodeId',
                'barcodeType',
                'barcodeName',
            ],
        ];
    }
}

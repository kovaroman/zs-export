<?hh //strict
namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class KuponaDE
 * @package ElasticExport\ResultFields
 */
class KuponaDE extends ResultFields
{
    /*
	 * @var ArrayHelper
	 */
private ArrayHelper $arrayHelper;

    /**
     * KuponaDE constructor.
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
                    'urlContent'
                ],
            ],

            'variationImageList' => [
                'params' => [
                    'type' => 'variation',
                    'referenceMarketplace' => $settings->get('referrerId') ? $settings->get('referrerId') : -1,
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
                'model',
                'limitOrderByStockSelect',
                'unitId',
                'content',
            ],

            'variationRetailPrice' => [
                'price',
            ],

            'variationRecommendedRetailPrice' => [
                'price',
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

            'variationBarcode' => [
                'params' => [
                    'barcodeType' => $settings->get('barcode') ? $settings->get('barcode') : 'EAN',
                ],
                'fields' => [
                    'code',
                    'barcodeId',
                ]
            ],
        ];
    }
}

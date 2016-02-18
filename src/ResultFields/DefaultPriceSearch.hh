<?hh //strict
namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class DefaultPriceSearch
 * @package ElasticExport\ResultFields
 */
class DefaultPriceSearch extends ResultFields
{
    /*
	 * @var ArrayHelper
	 */
private ArrayHelper $arrayHelper;

    /**
     * DefaultPriceSearch constructor.
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
                    'type' => 'variation',
                    'referenceMarketplace' => 2,
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
                'price',
                'retailPrice',
                'retailPriceNet',
                'basePrice',
                'basePriceNet',
                'unitPrice',
                'unitPriceNet',
            ],

            'variationStandardCategory' => [
                'params' => [
                    'plentyId' => $settings->get('plentyId') ? $settings->get('plentyId') : 1000,
                ],
                'fields' => [
                    'categoryId'
                ],
            ],

            'variationBarcode' => [
                'params' => [
                    'barcodeType' => $settings->get('barcode'),
                ],
                'fields' => [
                    'code',
                    'barcodeId',
                ]
            ],
        ];
    }
}

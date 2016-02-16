<?hh // strict
namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class Idealo
 * @package ElasticExport\ResultFields
 */
class Idealo extends ResultFields
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
                    'language' => $settings->get('lang'),
                ],
                'fields' => [
                    ($settings->get('nameId')) ? 'name' . $settings->get('nameId') : 'name1',
                    ($settings->get('descriptionType') == 'itemShortDescription') ? 'shortDescription' : '',
                    ($settings->get('descriptionType') == 'itemDescription' || $settings->get('descriptionType') == 'itemDescriptionAndTechnicalData') ? 'description' : '',
                    ($settings->get('descriptionType') == 'technicalData' || $settings->get('descriptionType') == 'itemDescriptionAndTechnicalData') ? 'technicalData' : '',
                ],
            ],

            'variationImageList' => [
                'params' => [
                    'type' => 'variation',
                    'referenceMarketplace' => $settings->get('reffererId'),
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

            'variationRecommendedRetailPrice' => [
                    'price', //[float]
                    'retailPrice', //[float]
                    'retailPriceNet', //[float]
                    'basePrice', //[float]
                    'basePriceNet', //[float]
                    'unitPrice', //[float]
                    'unitPriceNet', //[float]
                    'orderParamsMarkup', //[float]
                    'orderParamsMarkupNet', //[float]
                    'vatId', //[integer]
                    'vatValue', //[float]
                    'currency', //[mixed]
                    'exchangeRatio', //[float]
                    'minimumOrderQuantity', //[float]
                    'lastUpdateTimestamp' //[mixed]
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
                    'plentyId' => $settings->get('plentyId'),
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

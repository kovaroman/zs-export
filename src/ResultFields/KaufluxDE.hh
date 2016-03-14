<?hh // strict
namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class KaufluxDE
 * @package ElasticExport\ResultFields
 */
class KaufluxDE extends ResultFields
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

        $itemDescriptionFields = [
            'urlContent',
            'keywords',
        ];

        $itemDescriptionFields[] = ($settings->get('nameId')) ? 'name' . $settings->get('nameId') : 'name1';

        if($settings->get('descriptionType') == 'itemShortDescription')
        {
            $itemDescriptionFields[] = 'shortDescription';
        }

        if($settings->get('descriptionType') == 'itemDescription' || $settings->get('descriptionType') == 'itemDescriptionAndTechnicalData')
        {
            $itemDescriptionFields[] = 'description';
        }

        if($settings->get('descriptionType') == 'technicalData' || $settings->get('descriptionType') == 'itemDescriptionAndTechnicalData')
        {
            $itemDescriptionFields[] = 'technicalData';
        }

        return [
            'itemBase'=> [
                'id',
                'producer',
                'storeSpecial',
                'free1',
                'free2',
                'free3',
            ],

            'itemDescription' => [
                'params' => [
                    'language' => $settings->get('lang') ? $settings->get('lang') : 'de',
                ],
                'fields' => $itemDescriptionFields,
            ],

            'variationImageList' => [
                'params' => [
                    'type' => 'variation',
                ],
                'fields' => [
                    'type',
                    'path',
                    'position',
                ]
            ],

            'variationRecommendedRetailPrice' => [
                'price', //[float]
            ],

            'variationBase' => [
                'id',
                'availability',
                'attributeValueSetId',
                'model',
                'weightG',
                'vatId',
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

            'variationBarcodeList' => [
                'variationId',
                'code',
                'barcodeId',
                'barcodeType',
                'barcodeName',
            ],

            'variationRetailPrice' => [
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

            'itemCharacterList' => [
                 'itemCharacterId',
                 'characterId',
                 'characterValue',
                 'characterValueType',
            ],

            'itemCrossSellingList' => [
                 'itemId',
                 'crossItemId',
            ],

            'variationMarketStatus' => [
                'params' => [
                    'marketId' => $settings->get('referrerId'),
                ],
                'fields' => [
                    'id',
                    'marketId',
                    'marketAccountId',
                    'initialSku',
                    'sku',
                    'active',
                    'createdTimestamp',
                    'lastExportTimestamp',
                    'deletedTimestamp',
                    'marketStatus',
                    'additionalInformation',
                ],
            ],
        ];
    }
}
<?hh //strict
namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class Check24DE
 * @package ElasticExport\ResultFields
 */
class Check24DE extends ResultFields
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

        $itemDescriptionFields = ['urlContent'];
        $itemDescriptionFields[] = ($settings->get('nameId')) ? 'name' . $settings->get('nameId') : 'name1';

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
                'id',
                'availability',
                'attributeValueSetId',
                'model',
                'limitOrderByStockSelect',
                'unitId',
                'content',
                'weightG',
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

            'variationBarcode' => [
                'params' => [
                    'barcodeType' => $settings->get('barcode') ? $settings->get('barcode') : 'EAN',
                ],
                'fields' => [
                    'code',
                    'barcodeId',
                ]
            ],

            'variationMarketStatus' => [
                'params' => [
                    'marketId' => $settings->get('referrerId')
                ],
                'fields' => [
                    'sku'
                ]
            ],

            'variationStock' => [
                'params' => [
                    'type' => 'virtual'
                ],
                'fields' => [
                    'stockNet'
                ]
            ],

            'variationBarcodeList' => [
                'variationId',
                'code',
                'barcodeId',
                'barcodeType',
                'barcodeName',
            ],

            'variationAttributeValueList' => [
                'attributeId',
                'attributeValueId',
            ],
        ];
    }
}

<?hh //strict
namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class TreepodiaCOM
 * @package ElasticExport\ResultFields
 */
class TreepodiaCOM extends ResultFields
{
    /*
	 * @var ArrayHelper
	 */
	private ArrayHelper $arrayHelper;

    /**
     * Geizhals constructor.
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

        $itemDescriptionFields = [
            'urlContent',
            'keywords',
        ];

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

        $fields = [
            'itemBase'=> [
                'id',
                'producerId',
                'free1',
                'free2',
                'free3',
                'free4',
            ],

            'itemDescription' => [
                'params' => [
                    'language' => $settings->get('lang') ? $settings->get('lang') : 'de',
                ],
                'fields' => $itemDescriptionFields,
            ],

            'variationImageList' => [
                'params' => [
                    'type' => 'item_variation',
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

            'variationStock' => [
                'params' => [
                    'type' => 'virtual',
                ],
                'fields' => [
                    'stockNet'
                ]
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

            'variationCategoryList' => [
                'categoryId',
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

            'variationAttributeValueList' => [
                'attributeId',
                'attributeValueId',
            ],
        ];


        return $fields;
    }
}

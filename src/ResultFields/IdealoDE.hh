<?hh // strict
namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class IdealoDE
 * @package ElasticExport\ResultFields
 */
class IdealoDE extends ResultFields
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

        if($settings->get('variations') == 'mainVariations')
        {
            $this->setGroupByList(['groupBy.itemIdGetPrimaryVariation']);

        }

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
                'variationCount'
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
                    'referenceMarketplace' => 121,
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
                'limitOrderByStockSelect',
                'unitId',
                'content',
                'customNumber',
                'weightG',
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

            'variationMarketStatus' => [
                'params' => [
                    'marketId' => 121
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

            'variationAttributeValueList' => [
                'attributeId',
                'attributeValueId'
            ],
        ];
    }
}

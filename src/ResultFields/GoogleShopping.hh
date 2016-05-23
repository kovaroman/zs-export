<?hh //strict
namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class GoogleShopping
 * @package ElasticExport\ResultFields
 */
class GoogleShopping extends ResultFields
{
	/*
	 * @var ArrayHelper
	 */
	private ArrayHelper $arrayHelper;

	/**
	 * GoogleShopping constructor.
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

		$fields = [
			'itemBase'=> [
				'id',
				'producer',
				'condition',
			],

			'itemDescription' => [
				'params' => [
					'language' => $settings->get('lang') ? $settings->get('lang') : 'de',
				],
				'fields' => $itemDescriptionFields,
			],

			'variationImageList' => [
				'params' => [
					'type' => 'all',
					'referenceMarketplace' => $settings->get('referrerId') ? $settings->get('referrerId') : 7,
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

			'variationSpecialOfferRetailPrice' => array(
					'retailPriceId', //[integer]
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
					'lastUpdateTimestamp' //[mixed]

			),
			'variationRetailPrice' => [
				'price',
			],

			'variationStandardCategory' => [
				'params' => [
					'plentyId' => $settings->get('plentyId') ? $settings->get('plentyId') : 1000,
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


		return $fields;
	}
}

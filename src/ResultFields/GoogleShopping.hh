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
					'type' => 'variation',
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
			],

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

			'variationAttributeValueList' => [
				'attributeId',
				'attributeValueId',
			],
		];


		return $fields;
	}
}

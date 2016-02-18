<?hh //strict
namespace ElasticExport\ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

/**
 * Class Rakuten
 * @package ElasticExport\ResultFields
 */
class Rakuten extends ResultFields
{
	/*
	 * @var ArrayHelper
	 */
	private ArrayHelper $arrayHelper;

	/**
	 * Rakuten constructor.
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

		$fields = [
			'itemBase'=> [
				'id',
				'producer',
				'condition',
				'free1',
				'free2',
				'free3',
				'free4',
				'free5',
				'free6',
				'free7',
				'free8',
				'free9',
				'free10',
				'free11',
				'free12',
				'free13',
				'free14',
				'free15',
				'free16',
				'free17',
				'free18',
				'free19',
				'free20',
			],

			'itemDescription' => [
				'params' => [
					'language' => $settings->get('lang') ? $settings->get('lang') : 'de',
				],
				'fields' => [
					'urlContent',
					'shortDescription',
					'description',
					'technicalData',
				],
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
				'customNumber',
				'vatId',
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

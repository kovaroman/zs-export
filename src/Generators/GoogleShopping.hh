<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Unit\Models\UnitLang;
use Plenty\Modules\Helper\Models\KeyValue;

class GoogleShopping extends CSVGenerator
{
	const string CHARACTER_TYPE_GENDER						= 'gender';
	const string CHARACTER_TYPE_AGE_GROUP					= 'age_group';
	const string CHARACTER_TYPE_SIZE_TYPE					= 'size_type';
	const string CHARACTER_TYPE_SIZE_SYSTEM					= 'size_system';
	const string CHARACTER_TYPE_ENERGY_EFFICIENCY_CLASS		= 'energy_efficiency_class';
	const string CHARACTER_TYPE_EXCLUDED_DESTINATION		= 'excluded_destination';
	const string CHARACTER_TYPE_ADWORDS_REDIRECT			= 'adwords_redirect';
	const string CHARACTER_TYPE_MOBILE_LINK					= 'mobile_link';
	const string CHARACTER_TYPE_SALE_PRICE_EFFECTIVE_DATE	= 'sale_price_effective_date';
	const string CHARACTER_TYPE_CUSTOM_LABEL_0				= 'custom_label_0';
	const string CHARACTER_TYPE_CUSTOM_LABEL_1				= 'custom_label_1';
	const string CHARACTER_TYPE_CUSTOM_LABEL_2				= 'custom_label_2';
	const string CHARACTER_TYPE_CUSTOM_LABEL_3				= 'custom_label_3';
	const string CHARACTER_TYPE_CUSTOM_LABEL_4				= 'custom_label_4';
	const string CHARACTER_TYPE_DESCRIPTION					= 'description';
	const string CHARACTER_TYPE_COLOR						= 'color';
	const string CHARACTER_TYPE_SIZE						= 'size';
	const string CHARACTER_TYPE_PATTERN						= 'pattern';
	const string CHARACTER_TYPE_MATERIAL					= 'material';

	/*
	 * @var ElasticExportHelper
	 */
	private ElasticExportHelper $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
	private ArrayHelper $arrayHelper;

	/**
	 * GoogleShopping constructor.
	 * @param ElasticExportHelper $elasticExportHelper
	 * @param ArrayHelper $arrayHelper
	 */
	public function __construct(ElasticExportHelper $elasticExportHelper, ArrayHelper $arrayHelper)
	{
		$this->elasticExportHelper = $elasticExportHelper;
		$this->arrayHelper = $arrayHelper;
	}

	/**
	 * @param mixed $resultData
	 */
	protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
	{
		if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(";");

			$this->addCSVContent([
				'id',
				'title',
				'description',
				'google_product_category',
				'product_type',
				'link',
				'image_link',
				'condition',
				'availability',
				'price',
				'sale_price',
				'brand',
				'ean',
				'isbn',
				'mpn',
				'color',
				'size',
				'material',
				'pattern',
				'item_group_id',
				'shipping',
				'shipping_weight',
				'gender',
				'age_group',
				'excluded_destination',
				'adwords_redirect',
				'identifier_exists',
				'unit_pricing_measure',
				'unit_pricing_base_measure',
				'energy_efficiency_class',
				'size_system',
				'size_type',
				'mobile_link',
				'sale_price_effective_date',
				'adult',
				'custom_label_0',
				'custom_label_1',
				'custom_label_2',
				'custom_label_3',
				'custom_label_4',
			]);

			foreach($resultData as $item)
			{
				$data = [
					'id' 						=> $item->variationBase->id,
					'title' 					=> $this->elasticExportHelper->getName($item, $settings, 150),
					'description'				=> $this->getDescription($item, $settings),
					'google_product_category'	=> '',
					'product_type'				=> $this->elasticExportHelper->getCategory($item, $settings),
					'link'						=> $this->elasticExportHelper->getUrl($item, $settings, true, false),
					'image_link'				=> $this->elasticExportHelper->getMainImage($item, $settings),
					'condition'					=> $this->getCondition($item->itemBase->condition),
					'availability'				=> $this->elasticExportHelper->getAvailability($item, $settings),
					'price'						=> number_format($this->elasticExportHelper->getPrice($item, $settings), 2, '.', ''),
					'sale_price'				=> number_format($this->elasticExportHelper->getPrice($item, $settings), 2, '.', ''),
					'brand'						=> $item->itemBase->producer,
					'ean'						=> $item->variationBarcode->code,
					'isbn'						=> $item->variationBarcode->code,
					'mpn'						=> $item->variationBase->model,
					'color'						=> '',
					'size'						=> '',
					'material'					=> '',
					'pattern'					=> '',
					'item_group_id'				=> $item->itemBase->id,
					'shipping'					=> number_format($this->elasticExportHelper->getShippingCost($item, $settings), 2, ',', ''),
					'shipping_weight'			=> '',
					'gender'					=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_GENDER),
					'age_group'					=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_AGE_GROUP),
					'excluded_destination'		=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_EXCLUDED_DESTINATION),
					'adwords_redirect'			=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_ADWORDS_REDIRECT),
					'identifier_exists'			=> '',
					'unit_pricing_measure'		=> '',
					'unit_pricing_base_measure'	=> '',
					'energy_efficiency_class'	=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_ENERGY_EFFICIENCY_CLASS),
					'size_system'				=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_SIZE_SYSTEM),
					'size_type'					=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_SIZE_TYPE),
					'mobile_link'				=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_MOBILE_LINK),
					'sale_price_effective_date'	=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_SALE_PRICE_EFFECTIVE_DATE),
					'adult'						=> '',
					'custom_label_0'			=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_CUSTOM_LABEL_0),
					'custom_label_1'			=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_CUSTOM_LABEL_1),
					'custom_label_2'			=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_CUSTOM_LABEL_2),
					'custom_label_3'			=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_CUSTOM_LABEL_3),
					'custom_label_4'			=> $this->getCharacterValue($item, $settings, self::CHARACTER_TYPE_CUSTOM_LABEL_4),
				];

				$this->addCSVContent(array_values($data));
			}
		}
	}

	/**
	 * Check if condition is valid.
	 * @param int $condition
	 * @return string
	 */
	private function getCondition(int $conditionId):string
	{
		$conditionList = [
			0 => 'new',
			1 => 'used',
			2 => 'new',
			3 => 'new',
			4 => 'refurbished',
		];

		if (array_key_exists($conditionId, $conditionList))
		{
			return $conditionList[$conditionId];
		}
		else
		{
			return '';
		}
	}


	/**
	 * Check if gender is valid.
	 * @param Record $item
	 * @param KeyValue $settings
	 * @param string $type
	 * @return string
	 */
	private function getCharacterValue(Record $item, KeyValue $settings, string $type):string
	{
		$characterValue = $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, $type);

		switch($type)
		{
			case self::CHARACTER_TYPE_GENDER:
				$allowedList = [
					'male',
					'female',
					'unisex',
				];
				break;

			case self::CHARACTER_TYPE_AGE_GROUP:
				$allowedList = [
					'newborn',
					'infant',
					'toddler',
					'adult',
					'kids',
				];
				break;

			case self::CHARACTER_TYPE_SIZE_TYPE:
				$allowedList = [
					'regular',
					'petite',
					'plus',
					'maternity',
				];
				break;

			case self::CHARACTER_TYPE_SIZE_SYSTEM:
				$allowedList = [
					'US',
					'UK',
					'EU',
					'DE',
					'FR',
					'JP',
					'CN',
					'IT',
					'BR',
					'MEX',
					'AU',
				];
				break;

			case self::CHARACTER_TYPE_ENERGY_EFFICIENCY_CLASS:
				$allowedList = [
					'G',
					'F',
					'E',
					'D',
					'C',
					'B',
					'A',
					'A+',
					'A++',
					'A+++',
				];
				break;
		}

		if (in_array($characterValue, $allowedList))
		{
			return $characterValue;
		}
		else
		{
			return '';
		}
	}

	/**
	 * Get item description.
	 * @param Record $item
	 * @param KeyValue $settings
	 * @return string
	 */
	private function getDescription(Record $item, KeyValue $settings):string
	{
		$description = $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, self::CHARACTER_TYPE_DESCRIPTION);

		if (strlen($description) <= 0)
		{
			$description = $this->elasticExportHelper->getDescription($item, $settings, 5000);
		}

		if (strlen($description) <= 0)
		{
			$description = '';
		}

		return $description;
	}
}

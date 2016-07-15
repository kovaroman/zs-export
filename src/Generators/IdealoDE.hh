<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Order\Shipping\DefaultShipping\Models\DefaultShipping;
use Plenty\Modules\Order\Payment\Method\Models\PaymentMethod;
use Plenty\Modules\Item\Character\Contracts\CharacterSelectionRepositoryContract;
use Plenty\Modules\Item\Character\Models\CharacterSelection;

/**
 * Class IdealoDE
 * @package ElasticExport\Generators
 */
class IdealoDE extends CSVGenerator
{
	const string DEFAULT_PAYMENT_METHOD = 'vorkasse';

	const string SHIPPING_COST_TYPE_FLAT = 'flat';
    const string SHIPPING_COST_TYPE_CONFIGURATION = 'configuration';

	/**
     * @var ElasticExportHelper $elasticExportHelper
     */
    private ElasticExportHelper $elasticExportHelper;


	/**
	 * CharacterSelectionRepositoryContract $characterSelectionRepository
	 */
	private CharacterSelectionRepositoryContract $characterSelectionRepository;

	/**
	 * @var array<int,mixed>
	 */
	private array<int,array<string,string>>$itemPropertyCache = [];

	/**
	 * @var ArrayHelper $arrayHelper
	 */
	private ArrayHelper $arrayHelper;

	/**
	 * @var Map<int,PaymentMethod> $usedPaymentMethods
	 */
	private Map<int,PaymentMethod> $usedPaymentMethods = Map{};

    /**
     * IdealoGenerator constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
	 * @param CharacterSelectionRepositoryContract $characterSelectionRepository
	 */
    public function __construct(
		ElasticExportHelper $elasticExportHelper,
		ArrayHelper $arrayHelper,
		CharacterSelectionRepositoryContract $characterSelectionRepository
	)
    {
        $this->elasticExportHelper = $elasticExportHelper;
		$this->arrayHelper = $arrayHelper;
		$this->characterSelectionRepository = $characterSelectionRepository;
    }

    protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
    {
        if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter("	"); // tab not space!

			$this->addCSVContent($this->head($settings));

			foreach($resultData as $item)
			{
				$attributes = $this->elasticExportHelper->getAttributeValueSetShortFrontendName($item, $settings, '|');
				if(strlen($attributes) <= 0 && $item->itemBase->variationCount > 1)
				{
					continue;
				}
				$this->addCSVContent($this->row($item, $settings));
			}
		}
    }

	private function head(KeyValue $settings):array<string>
	{
		$data = [
			'article_id',
			'deeplink',
			'name',
			'short_description',
			'description',
			'article_no',
			'producer',
			'model',
			'availability',
			'ean',
			'isbn',
			'fedas',
			'warranty',
			'price',
			'price_old',
			'weight',
			'category1',
			'category2',
			'category3',
			'category4',
			'category5',
			'category6',
			'category_concat',
			'image_url_preview',
			'image_url',
			'base_price',
			'free_text_field',
			'checkoutApproved',
			'itemsInStock',
			'fulfillmentType',
			'twoManHandlingPrice',
			'disposalPrice'
		];

		if($settings->get('shippingCostType') == self::SHIPPING_COST_TYPE_CONFIGURATION)
		{
			$paymentMethods = $this->elasticExportHelper->getPaymentMethods($settings);

			$defaultShipping = $this->elasticExportHelper->getDefaultShipping($settings);

			if($defaultShipping instanceof DefaultShipping)
			{
				foreach([$defaultShipping->paymentMethod1, $defaultShipping->paymentMethod2, $defaultShipping->paymentMethod3] as $paymentMethodId)
				{
					if(array_key_exists($paymentMethodId, $paymentMethods) && !array_key_exists($paymentMethodId, $this->usedPaymentMethods))
					{
						$data[] = $this->getPaymentMethodName($paymentMethods[$paymentMethodId], $settings->get('lang') ?: 'de');
						$this->usedPaymentMethods[$paymentMethodId] = $paymentMethods[$paymentMethodId];
					}
				}
			}
		}

		if(count($this->usedPaymentMethods) <= 0)
		{
			$data[] = self::DEFAULT_PAYMENT_METHOD;
		}

		return $data;
	}

	/**
	 * Get item row.
	 * @param Record   $item
	 * @param KeyValue $settings
	 * @return array<string>
	 */
	private function row(Record $item, KeyValue $settings):array<mixed>
	{
		$price = $this->elasticExportHelper->getPrice($item) <= 0 ? $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) : $this->elasticExportHelper->getPrice($item);
		$rrp = $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) <= $price ? 0 : $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings);

		$variationName = $this->elasticExportHelper->getAttributeValueSetShortFrontendName($item, $settings);

		if($item->variationBase->limitOrderByStockSelect == 2)
		{
			$stock = 999;
		}
		elseif($item->variationBase->limitOrderByStockSelect == 1 && $item->variationStock->stockNet > 0)
		{
			if($item->variationStock->stockNet > 999)
			{
				$stock = 999;
			}
			else
			{
				$stock = $item->variationStock->stockNet;
			}
		}
		elseif($item->variationBase->limitOrderByStockSelect == 0)
		{
			if($item->variationStock->stockNet > 999)
			{
				$stock = 999;
			}
			else
			{
				if($item->variationStock->stockNet > 0)
				{
					$stock = $item->variationStock->stockNet;
				}
				else
				{
					$stock = 0;
				}
			}
		}
		else
		{
			$stock = 0;
		}

		$checkoutApproved = $this->getProperty($item, 'CheckoutApproved');
		if(is_null($checkoutApproved))
		{
			$checkoutApproved = 'false';
		}
		else
		{
			$checkoutApproved = 'true';
		}

		$data = [
			'article_id' 		=> $this->elasticExportHelper->generateSku($item, 121, $item->variationMarketStatus->sku),
			'deeplink' 			=> $this->elasticExportHelper->getUrl($item, $settings, true, false),
			'name' 				=> $this->elasticExportHelper->getName($item, $settings) . (strlen($variationName) ? ' ' . $variationName : ''),
			'short_description' => $this->elasticExportHelper->getPreviewText($item, $settings),
			'description' 		=> $this->elasticExportHelper->getDescription($item, $settings),
			'article_no' 		=> $item->variationBase->customNumber,
			'producer' 			=> $item->itemBase->producer,
			'model' 			=> $item->variationBase->model,
			'availability' 		=> $this->elasticExportHelper->getAvailability($item, $settings),
			'ean'	 			=> $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_EAN),
			'isbn' 				=> $this->elasticExportHelper->getBarcodeByType($item, $settings, ElasticExportHelper::BARCODE_ISBN),
			'fedas' 			=> $item->itemBase->fedas,
			'warranty' 			=> '',
			'price' 			=> number_format($price, 2, '.', ''),
			'price_old' 		=> number_format($rrp, 2, '.', ''),
			'weight' 			=> $item->variationBase->weightG,
			'category1' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 1),
			'category2' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 2),
			'category3' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 3),
			'category4' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 4),
			'category5' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 5),
			'category6' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 6),
			'category_concat' 	=> $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
			'image_url_preview' => $this->getImages($item, $settings, ';', 'preview'),
			'image_url' 		=> $this->getImages($item, $settings, ';', 'normal'),
			'base_price' 		=> $this->elasticExportHelper->getBasePrice($item, $settings),
			'free_text_field'   => $this->getFreeText($item),
			'checkoutApproved'	=> $checkoutApproved,

		];

		/**
		 * if the article is available for idealo DK further fields will be set depending on the properties of the article.
		 *
		 * Be sure to set the price in twoManHandlingPrice and disposalPrice with a dot instead of a comma for idealo DK
		 * will only except it that way.
		 *
		 * The properties twoManHandlingPrice and disposalPrice will also only be set if the property fulfillmentType is 'Spedition'
		 * otherwise these two properties will be ignored.
		*/
		if($checkoutApproved == 'true')
		{
			$data['itemsInStock'] = $stock;
			$data['fulfillmentType'] = $this->getProperty($item, 'FulfillmentType');
			if($data['fulfillmentType'] == 'Spedition')
			{
				$twoManHandling = $this->getProperty($item, 'TwoManHandlingPrice');
				$disposal = $this->getProperty($item, 'DisposalPrice');

				strlen($twoManHandling) > 0 ?
					$data['twoManHandlingPrice'] = $twoManHandling : $data['twoManHandlingPrice'] = '';
				strlen($disposal) > 0 ?
					$data['disposalPrice'] = $disposal : $data['disposalPrice'] = '';
			}
			else
			{
				$data['itemsInStock'] = '';
				$data['twoManHandlingPrice'] = '';
				$data['disposalPrice'] = '';
			}
		}

		if($settings->get('shippingCostType') == self::SHIPPING_COST_TYPE_CONFIGURATION)
		{
			$defaultShipping = $this->elasticExportHelper->getDefaultShipping($settings);

			if($defaultShipping instanceof DefaultShipping && count($this->usedPaymentMethods))
			{
				foreach($this->usedPaymentMethods as $paymentMethod)
				{
					$name = $this->getPaymentMethodName($paymentMethod, $settings->get('lang') ?: 'de');
					$cost = $this->elasticExportHelper->calculateShippingCost($item->itemBase->id, $defaultShipping->shippingDestinationId, $settings->get('referrerId'), $paymentMethod->id);

					$data[$name] = number_format($cost, 2, '.', '');
				}
			}
		}

		if(count($this->usedPaymentMethods) <= 0)
		{
			$data[self::DEFAULT_PAYMENT_METHOD] = 0.00;
		}

		return array_values($data);
	}

	/**
	 * Get free text.
	 * @param  Record   $item
	 * @return {string
	 */
	private function getFreeText(Record $item):string
	{
		$characterMarketComponentList = $this->elasticExportHelper->getItemCharactersByComponent($item, 121.00 , 1);

		$freeText = [];

		if(count($characterMarketComponentList))
		{
			foreach($characterMarketComponentList as $data)
			{
				if((string) $data['characterValueType'] != 'file' && (string) $data['characterValueType'] != 'empty')
				{
					$freeText[] = (string) $data['characterValue'];
				}
			}
		}

		return implode(' ', $freeText);
	}

	/**
     * Get images.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  string   $separator  = ','
     * @param  string   $imageType  = 'normal'
     * @return string
     */
    private function getImages(Record $item, KeyValue $settings, string $separator = ',', string $imageType = 'normal'):string
    {
        $list = $this->elasticExportHelper->getImageList($item, $settings, $imageType);

        if(count($list))
        {
            return implode($separator, $list);
        }

        return '';
    }

	/**
	 * Get payment method name.
	 * @param  PaymentMethod $paymentMethod
	 * @return string
	 */
	private function getPaymentMethodName(PaymentMethod $paymentMethod, string $lang = 'de'):string
	{
		foreach($paymentMethod->informations as $paymentMethodInformation)
		{
			if($paymentMethodInformation->lang == $lang)
			{
				return $paymentMethodInformation->name;
			}
		}

		return '';
	}

	    /**
		 * Get property.
		 * @param  Record   $item
		 * @param  string   $property
		 * @return string|null
		 */
    private function getProperty(Record $item, string $property):?string
	{
		$itemPropertyList = $this->getItemPropertyList($item, 121.00);

		if(array_key_exists($property, $itemPropertyList))
		{
			return $itemPropertyList[$property];
		}

		return null;
	}

    /**
	 * Get item properties.
	 * @param 	Record $item
	 * @param   float $marketId
	 * @return  array<string,string>
	 */
    private function getItemPropertyList(Record $item, float $marketId):array<string,string>
    {
		if(!array_key_exists($item->itemBase->id, $this->itemPropertyCache))
		{
			$characterMarketComponentList = $this->elasticExportHelper->getItemCharactersByComponent($item, $marketId);

			$list = [];

			if(count($characterMarketComponentList))
			{
				foreach($characterMarketComponentList as $data)
				{
					if((string) $data['characterValueType'] != 'file')
					{
						if((string) $data['characterValueType'] == 'selection')
						{
							$characterSelection = $this->characterSelectionRepository->findCharacterSelection((int) $data['characterValue']);
							if($characterSelection instanceof CharacterSelection)
							{
								$list[(string) $data['externalComponent']] = (string) $characterSelection->name;
							}
						}
						else
						{
							$list[(string) $data['externalComponent']] = (string) $data['characterValue'];
						}

					}
				}
			}

			$this->itemPropertyCache[$item->itemBase->id] = $list;
		}

		return $this->itemPropertyCache[$item->itemBase->id];
	}
}

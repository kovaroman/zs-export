<?php

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Order\Shipping\Models\DefaultShipping;
use Plenty\Modules\Order\Payment\Method\Models\PaymentMethod;
use Plenty\Modules\Item\Property\Contracts\PropertySelectionRepositoryContract;
use Plenty\Modules\Item\Property\Models\PropertySelection;

/**
 * Class IdealoDE
 * @package ElasticExport\Generators
 */
class IdealoDE extends CSVGenerator
{
	const DEFAULT_PAYMENT_METHOD = 'vorkasse';

	const SHIPPING_COST_TYPE_FLAT = 'flat';
    const SHIPPING_COST_TYPE_CONFIGURATION = 'configuration';

	/**
     * @var ElasticExportHelper $elasticExportHelper
     */
    private $elasticExportHelper;


	/**
	 * PropertySelectionRepositoryContract $propertySelectionRepository
	 */
	private $propertySelectionRepository;

	/**
	 * @var array
	 */
	private $itemPropertyCache = [];

	/**
	 * @var ArrayHelper $arrayHelper
	 */
	private $arrayHelper;

    /**
     * @var array
     */
    private $usedPaymentMethods = [];

    /**
     * @var array
     */
    private $defaultShippingList = [];

    /**
     * IdealoGenerator constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
	 * @param PropertySelectionRepositoryContract $propertySelectionRepository
	 */
    public function __construct(
		ElasticExportHelper $elasticExportHelper,
		ArrayHelper $arrayHelper,
		PropertySelectionRepositoryContract $propertySelectionRepository
	)
    {
        $this->elasticExportHelper = $elasticExportHelper;
		$this->arrayHelper = $arrayHelper;
		$this->propertySelectionRepository = $propertySelectionRepository;
    }

	/**
	 * @param RecordList $resultData
	 * @param array $formatSettings
	 */
    protected function generateContent($resultData, array $formatSettings = [])
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

	private function head(KeyValue $settings):array
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

        /**
         * If the shipping cost type is configuration, all payment methods will be taken as available payment methods from the chosen
         * default shipping configuration.
         */
        if($settings->get('shippingCostType') == self::SHIPPING_COST_TYPE_CONFIGURATION)
        {
            $paymentMethods = $this->elasticExportHelper->getPaymentMethods($settings);

            $defaultShipping = $this->elasticExportHelper->getDefaultShipping($settings);

            if($defaultShipping instanceof DefaultShipping)
            {
                foreach([$defaultShipping->paymentMethod2, $defaultShipping->paymentMethod3] as $paymentMethodId)
                {
                    if(count($this->usedPaymentMethods) == 0 && array_key_exists($paymentMethodId, $paymentMethods))
                    {
                        $data[] = $paymentMethods[$paymentMethodId]->getAttributes()['name'];
                        $this->usedPaymentMethods[$defaultShipping->id][] = $paymentMethods[$paymentMethodId];
                    }
                    elseif(array_key_exists($paymentMethodId, $paymentMethods) && count($this->usedPaymentMethods) == 1
                        && ($this->usedPaymentMethods[$defaultShipping->id][0]->getAttributes()['id'] != $paymentMethodId))
                    {
                        $data[] = $paymentMethods[$paymentMethodId]->getAttributes()['name'];
                        $this->usedPaymentMethods[$defaultShipping->id][] = $paymentMethods[$paymentMethodId];
                    }
                    elseif(array_key_exists($paymentMethodId, $paymentMethods) && count($this->usedPaymentMethods) == 2
                        && ($this->usedPaymentMethods[$defaultShipping->id][0]->getAttributes()['id'] != $paymentMethodId))
                    {
                        $data[] = $paymentMethods[$paymentMethodId]->getAttributes()['name'];
                        $this->usedPaymentMethods[$defaultShipping->id][] = $paymentMethods[$paymentMethodId];
                    }
                }
            }
        }
        /**
         * If nothing is checked at the elastic export settings regarding the shipping cost type,
         * all payment methods within both default shipping configurations will be taken as available payment methods.
         */
        elseif($settings->get('shippingCostType') == 1)
        {
            $paymentMethods = $this->elasticExportHelper->getPaymentMethods($settings);

            $this->defaultShippingList = $this->elasticExportHelper->getDefaultShippingList();

            foreach($this->defaultShippingList as $defaultShipping)
            {
                if($defaultShipping instanceof DefaultShipping)
                {
                    foreach([$defaultShipping->paymentMethod2, $defaultShipping->paymentMethod3] as $paymentMethodId)
                    {
                        if(count($this->usedPaymentMethods) == 0 && array_key_exists($paymentMethodId, $paymentMethods))
                        {
                            $data[] = $paymentMethods[$paymentMethodId]->getAttributes()['name'];
                            $this->usedPaymentMethods[$defaultShipping->id][] = $paymentMethods[$paymentMethodId];
                        }
                        elseif(array_key_exists($paymentMethodId, $paymentMethods) && count($this->usedPaymentMethods) == 1
                            && $this->usedPaymentMethods[1][0]->getAttributes()['id'] != $paymentMethodId)
                        {
                            $data[] = $paymentMethods[$paymentMethodId]->getAttributes()['name'];
                            $this->usedPaymentMethods[$defaultShipping->id][] = $paymentMethods[$paymentMethodId];
                        }

                        elseif(array_key_exists($paymentMethodId, $paymentMethods) && count($this->usedPaymentMethods) == 2
                            && ($this->usedPaymentMethods[1][0]->getAttributes()['id'] != $paymentMethodId && $this->usedPaymentMethods[2][0]->getAttributes()['id'] != $paymentMethodId))
                        {
                            $data[] = $paymentMethods[$paymentMethodId]->getAttributes()['name'];
                            $this->usedPaymentMethods[$defaultShipping->id][] = $paymentMethods[$paymentMethodId];
                        }
                    }
                }
            }
        }
        if(count($this->usedPaymentMethods) <= 0 || $settings->get('shippingCostType') == self::SHIPPING_COST_TYPE_FLAT)
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
	private function row(Record $item, KeyValue $settings):array
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

		if(is_null($checkoutApproved) || strlen($checkoutApproved) <= 0)
		{
			$checkoutApproved = 'false';
		}
		else
		{
			$checkoutApproved = 'true';
		}

		$data = [
			'article_id' 		=> '',
			'deeplink' 			=> $this->elasticExportHelper->getUrl($item, $settings, true, false),
			'name' 				=> $this->elasticExportHelper->getName($item, $settings) . (strlen($variationName) ? ' ' . $variationName : ''),
			'short_description' => $this->elasticExportHelper->getPreviewText($item, $settings),
			'description' 		=> $this->elasticExportHelper->getDescription($item, $settings),
			'article_no' 		=> $item->variationBase->customNumber,
			'producer' 			=> $this->elasticExportHelper->getExternalManufacturerName((int)$item->itemBase->producerId),
			'model' 			=> $item->variationBase->model,
			'availability' 		=> $this->elasticExportHelper->getAvailability($item, $settings),
			'ean'	 			=> $this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode')),
			'isbn' 				=> $this->elasticExportHelper->getBarcodeByType($item, ElasticExportHelper::BARCODE_ISBN),
			'fedas' 			=> $item->itemBase->fedas,
			'warranty' 			=> '',
			'price' 			=> number_format((float)$price, 2, '.', ''),
			'price_old' 		=> number_format((float)$rrp, 2, '.', ''),
			'weight' 			=> $item->variationBase->weightG,
			'category1' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 1),
			'category2' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 2),
			'category3' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 3),
			'category4' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 4),
			'category5' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 5),
			'category6' 		=> $this->elasticExportHelper->getCategoryBranch($item, $settings, 6),
			'category_concat' 	=> $this->elasticExportHelper->getCategory((int)$item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
			'image_url_preview' => $this->elasticExportHelper->getMainImage($item, $settings, 'preview'),
			'image_url' 		=> $this->elasticExportHelper->getMainImage($item, $settings, 'normal'),
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
			$data['article_id'] = $this->elasticExportHelper->generateSku($item, 121.02, (string)$item->variationMarketStatus->sku);
			$data['itemsInStock'] = $stock;
			$fulfillmentType = $this->getProperty($item, 'FulfillmentType:Spedition');

			if(!is_null($fulfillmentType) || strlen($fulfillmentType) > 0)
			{
				$fulfillmentType = 'Spedition';
			}
			else
			{
				$full = $this->getProperty($item, 'FulfillmentType:Paketdienst');
				$fulfillmentType = is_null($full) || strlen($full) <= 0 ? '' : 'Paketdienst';
			}

			$data['fulfillmentType'] = $fulfillmentType;

			if($data['fulfillmentType'] == 'Spedition')
			{
				$twoManHandling = $this->getProperty($item, 'TwoManHandlingPrice');
				$twoManHandling = str_replace(",", '.', $twoManHandling);
				$twoManHandling = number_format((float)$twoManHandling, 2, ',', '');
				$disposal = $this->getProperty($item, 'DisposalPrice');
				$disposal = str_replace(",", '.', $disposal);
				$disposal = number_format((float)$disposal, 2, ',', '');

				$twoManHandling > 0 ?
					$data['twoManHandlingPrice'] = $twoManHandling : $data['twoManHandlingPrice'] = '';

				if($twoManHandling > 0)
				{
					$disposal > 0 ?
						$data['disposalPrice'] = $disposal : $data['disposalPrice'] = '';
				}
				else
				{
					$data['disposalPrice'] = '';
				}
			}
			else
			{
				$data['twoManHandlingPrice'] = '';
				$data['disposalPrice'] = '';
			}
		}
		else
		{
			$data['article_id'] = $this->elasticExportHelper->generateSku($item, 121, (string)$item->variationMarketStatus->sku);
			$data['itemsInStock'] = '';
			$data['fulfillmentType'] = '';
			$data['twoManHandlingPrice'] = '';
			$data['disposalPrice'] = '';
		}

        if(count($this->usedPaymentMethods) == 1)
        {
            foreach($this->usedPaymentMethods as $paymentMethod)
            {
                foreach($paymentMethod as $method)
                {
                    $name = $method->getAttributes()['name'];
                    $cost = $this->elasticExportHelper->getShippingCost($item, $settings, $method->id);
                    $data[$name] = number_format((float)$cost, 2, '.', '');
                }
            }
        }
        elseif(count($this->usedPaymentMethods) > 1)
        {
            foreach($this->usedPaymentMethods as $defaultShipping => $paymentMethod)
            {
                foreach ($paymentMethod as $method)
                {
                    $name = $method->getAttributes()['name'];
                    $cost = $this->elasticExportHelper->calculateShippingCost(
                        $item->itemBase->id,
                        $this->defaultShippingList[$defaultShipping]->shippingDestinationId,
                        $this->defaultShippingList[$defaultShipping]->referrerId,
                        $method->id);
                    $data[$name] = number_format((float)$cost, 2, '.', '');
                }
            }
        }
        elseif(count($this->usedPaymentMethods) <= 0 && $settings->get('shippingCostType') == self::SHIPPING_COST_TYPE_FLAT)
        {
            $data[self::DEFAULT_PAYMENT_METHOD] = $settings->get('shippingCostFlat');
        }
        else
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
     * Get property.
     * @param  Record   $item
     * @param  string   $property
     * @return string|null
     */
    private function getProperty(Record $item, string $property):string
	{
		$itemPropertyList = $this->getItemPropertyList($item, 121.00);

		if(array_key_exists($property, $itemPropertyList))
		{
			if ($property == "CheckoutApproved")
			{
				return true;
			}
			else
			{
				return $itemPropertyList[$property];
			}
		}

		return '';
	}

    /**
	 * Get item properties.
	 * @param 	Record $item
	 * @param   float $marketId
	 * @return  array<string,string>
	 */
    private function getItemPropertyList(Record $item, float $marketId):array
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
							$propertySelection = $this->propertySelectionRepository->findOne((int) $data['characterValue'], 'de');
							if($propertySelection instanceof PropertySelection)
							{
								$list[(string) $data['externalComponent']] = (string) $propertySelection->name;
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

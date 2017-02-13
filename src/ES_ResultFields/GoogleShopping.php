<?php

namespace ElasticExport\ES_ResultFields;

use Plenty\Modules\DataExchange\Contracts\ResultFields;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\Search\Mutators\ImageMutator;
use Plenty\Modules\Cloud\ElasticSearch\Lib\Source\Mutator\BuiltIn\LanguageMutator;
use Plenty\Modules\Item\Search\Mutators\SkuMutator;

/**
 * Class GoogleShopping
 * @package ElasticExport\ResultFields
 */
class GoogleShopping extends ResultFields
{
	/*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

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
	 * @param  array $formatSettings = []
	 * @return array
	 */
	public function generateResultFields(array $formatSettings = []):array
	{
		$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');
		$reference = $settings->get('referrerId') ? $settings->get('referrerId') : 7;

		$list = [];

		if($settings->get('nameId'))
		{
			$list[] = 'texts.name'.$settings->get('nameId');
		}
		else
		{
			$list[] = 'texts.name1';
		}

		if ($settings->get('descriptionType') == 'itemShortDescription' ||
			$settings->get('previewTextType') == 'itemShortDescription')
		{
			$list[] = 'texts.shortDescription';
		}

		if ($settings->get('descriptionType') == 'itemDescription' ||
			$settings->get('descriptionType') == 'itemDescriptionAndTechnicalData' ||
			$settings->get('previewTextType') == 'itemDescription' ||
			$settings->get('previewTextType') == 'itemDescriptionAndTechnicalData')
		{
			$list[] = 'texts.description';
		}

		if ($settings->get('descriptionType') == 'technicalData' ||
			$settings->get('descriptionType') == 'itemDescriptionAndTechnicalData' ||
			$settings->get('previewTextType') == 'technicalData' ||
			$settings->get('previewTextType') == 'itemDescriptionAndTechnicalData')
		{
			$list[] = 'texts.technicalData';
		}

		//Mutator
		/**
		 * @var ImageMutator $imageMutator
		 */
		$imageMutator = pluginApp(ImageMutator::class);
		$imageMutator->addMarket($reference);
		/**
		 * @var LanguageMutator $languageMutator
		 */
		$languageMutator = pluginApp(LanguageMutator::class, [[$settings->get('lang')]]);
		/**
		 * @var SkuMutator $skuMutator
		 */
		$skuMutator = pluginApp(SkuMutator::class);
		$skuMutator->setMarket($reference);

		$fields = [
			[
				// Item
				'item.id',
				'item.manufacturer.id',
				'item.apiCondition',
				'texts.urlPath',

				// Variation
				'id',
				'variation.availability.id',
				'variation.model',
				'variation.stockLimitation',
				'variation.weightG',

				// Images
				'images.all.type',
				'images.all.path',
				'images.all.position',

				// Unit
				'unit.content',
				'unit.id',

				//sku
				'skus.sku',

				// Default categories
				'defaultCategories.id',

				// Barcodes
				'barcodes.code',
				'barcodes.type',

				// Attributes
				'attributes.attributeValueSetId',
				'attributes.attributeId',
				'attributes.valueId',
			],

			[
				$imageMutator,
				$languageMutator,
				$skuMutator
			],


//			'itemBase'=> [
//				'id',
//				'producerId',
//				'apiCondition',
//			],

//			'itemDescription' => [
//				'params' => [
//					'language' => $settings->get('lang') ? $settings->get('lang') : 'de',
//				],
//				'fields' => $itemDescriptionFields,
//			],

//			'itemPropertyList' => [
//				'params' => [],
//				'fields' => [
//					'itemPropertyId',
//					'propertyId',
//					'propertyValue',
//					'propertyValueType',
//					'isOrderProperty',
//					'propertyOrderMarkup'
//				]
//			],

//			'variationImageList' => [
//				'params' => [
//					'itemImages' => [
//						'type'				 => 'item', // all images
//						'imageType'			=> ['internal'],
//						'referenceMarketplace' => $settings->get('referrerId') ? $settings->get('referrerId') : 7,
//					],
//					'variationImages'					=> [
//						'type'				 => 'variation', // current variation images
//						'imageType'			=> ['internal'],
//						'referenceMarketplace' => $settings->get('referrerId') ? $settings->get('referrerId') : 7,
//					],
//				],
//				'fields' => [
//					'type',
//					'path',
//					'position',
//				],
//			],

//			'variationBase' => [
//				'id',
//				'availability',
//				'attributeValueSetId',
//				'model',
//				'limitOrderByStockSelect',
//				'unitId',
//				'content',
//				'weightG',
//			],

//			'variationSpecialOfferRetailPrice' => [
//				'params' => [
//					'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : 7,
//				],
//				'fields' => [
//					'retailPriceId',
//					'retailPrice',
//					'retailPriceNet',
//					'basePrice',
//					'basePriceNet',
//					'unitPrice',
//					'unitPriceNet',
//					'orderParamsMarkup',
//					'orderParamsMarkupNet',
//					'vatId',
//					'vatValue',
//					'currency',
//					'exchangeRatio',
//					'lastUpdateTimestamp'
//				],
//			],
//			'variationRetailPrice' => [
//				'params' => [
//					'referrerId' => $settings->get('referrerId') ? $settings->get('referrerId') : 7,
//				],
//				'fields' => [
//					'price',
//				],
//			],

//			'variationStandardCategory' => [
//				'params' => [
//					'plentyId' => $settings->get('plentyId'),
//				],
//				'fields' => [
//					'categoryId',
//					'plentyId',
//					'manually',
//				],
//			],

//			'variationBarcodeList' => [
//				'params' => [
//					'barcodeType' => $settings->get('barcode') ? $settings->get('barcode') : 'EAN',
//				],
//				'fields' => [
//					'code',
//					'barcodeId',
//				]
//			],

//			'variationAttributeValueList' => [
//				'attributeId',
//				'attributeValueId',
//			],
		];

		if (is_array($list) && count($list) > 0)
		{
			foreach($list as $element)
			{
				$fields[0][] = $element;
			}
		}

		return $fields;
	}
}

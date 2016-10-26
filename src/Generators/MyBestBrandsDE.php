<?php

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Item\Attribute\Contracts\AttributeValueNameRepositoryContract;
use Plenty\Modules\Item\Attribute\Models\AttributeValueName;
use Plenty\Modules\Item\Property\Contracts\PropertySelectionRepositoryContract;
use Plenty\Modules\Item\Property\Models\PropertySelection;

class MyBestBrandsDE extends CSVGenerator
{
	/*
     * @var ElasticExportHelper
     */
    private $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

	/**
	 * AttributeValueNameRepositoryContract $attributeValueNameRepository
	 */
	private $attributeValueNameRepository;

	/**
	 * PropertySelectionRepositoryContract $propertySelectionRepository
	 */
	private $propertySelectionRepository;

	/**
	 * @var array
	 */
	private $itemPropertyCache = [];

	/**
     * Geizhals constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     * @param AttributeValueNameRepositoryContract $attributeValueNameRepository
     * @param PropertySelectionRepositoryContract $propertySelectionRepository
     */
    public function __construct(
    	ElasticExportHelper $elasticExportHelper,
    	ArrayHelper $arrayHelper,
    	AttributeValueNameRepositoryContract $attributeValueNameRepository,
    	PropertySelectionRepositoryContract $propertySelectionRepository
    )
    {
		$this->elasticExportHelper = $elasticExportHelper;
		$this->arrayHelper = $arrayHelper;
		$this->attributeValueNameRepository = $attributeValueNameRepository;
		$this->propertySelectionRepository = $propertySelectionRepository;
    }

	/**
	 * @param RecordList $resultData
	 * @param array
	 */
	protected function generateContent($resultData, array $formatSettings = [])
	{
		if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(";");

			$this->addCSVContent([
				'ProductID',
				'ProductCategory',
				'Deeplink',
				'ProductName',
				'ImageUrl',
				'ProductDescription',
				'BrandName',
				'Price',
				'PreviousPrice',
				'AvailableSizes',
				'Tags',
				'EAN',
				'LastUpdate',
				'UnitPrice',
				'RetailerAttributes',
				'Color',
			]);

			$rows = [];

			foreach($resultData as $item)
			{
				if(!array_key_exists($item->itemBase->id, $rows))
				{
					$rows[$item->itemBase->id] = $this->getMain($item, $settings);
				}

				if(array_key_exists($item->itemBase->id, $rows) && $item->variationBase->attributeValueSetId > 0)
				{
					$variationAttributes = $this->getVariationAttributes($item, $settings);

					if(array_key_exists('Color', $variationAttributes))
					{
						$rows[$item->itemBase->id]['Color'] = array_unique(array_merge($rows[$item->itemBase->id]['Color'], $variationAttributes['Color']));
					}

					if(array_key_exists('Size', $variationAttributes))
					{
						$rows[$item->itemBase->id]['AvailableSizes'] = array_unique(array_merge($rows[$item->itemBase->id]['AvailableSizes'], $variationAttributes['Size']));
					}
				}
				elseif(array_key_exists($item->itemBase->id, $rows))
				{
					$itemPropertyList = $this->getItemPropertyList($item);

					foreach($itemPropertyList as $key => $value)
					{
						switch($key)
						{
							case 'color':
								array_push($rows[$item->itemBase->id]['Color'], $value);
								$rows[$item->itemBase->id]['Color'] = array_unique($rows[$item->itemBase->id]['Color']);
								break;

							case 'available_sizes':
								array_push($rows[$item->itemBase->id]['AvailableSizes'], $value);
								$rows[$item->itemBase->id]['AvailableSizes'] = array_unique($rows[$item->itemBase->id]['AvailableSizes']);
								break;
						}
					}
				}
			}

			foreach($rows as $data)
			{
				if(array_key_exists('Color', $data) && is_array($data['Color']))
				{
					$data['Color'] = implode(';', $data['Color']);
				}

				if(array_key_exists('AvailableSizes', $data) && is_array($data['AvailableSizes']))
				{
					$data['AvailableSizes'] = implode(', ', $data['AvailableSizes']);
				}

				$this->addCSVContent(array_values($data));
			}
		}
	}

	/**
	 * Get main information.
	 * @param  Record $item
	 * @param  KeyValue $settings
	 * @return array
	 */
	private function getMain(Record $item, KeyValue $settings):array
	{
		$itemPropertyList = $this->getItemPropertyList($item);

		$productName = array_key_exists('itemName', $itemPropertyList) && strlen((string) $itemPropertyList['itemName']) ?
							$this->elasticExportHelper->cleanName((string) $itemPropertyList['itemName'], $settings->get('nameMaxLength')) :
							$this->elasticExportHelper->getName($item, $settings);

		$data = [
			'ProductID' 			=> $item->itemBase->id,
			'ProductCategory' 		=> str_replace(array('&', '/'), array('und', ' '), $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId'))),
			'Deeplink' 				=> $this->elasticExportHelper->getUrl($item, $settings, true, false),
			'ProductName'			=> $productName,
			'ImageUrl' 				=> $this->elasticExportHelper->getMainImage($item, $settings),
			'ProductDescription' 	=> $this->elasticExportHelper->getDescription($item, $settings),
			'BrandName'				=> $this->elasticExportHelper->getExternalManufacturerName($item->itemBase->producerId),
			'Price'					=> number_format($this->elasticExportHelper->getPrice($item), 2, ',', ''),
			'PreviousPrice'			=> number_format($this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) > $this->elasticExportHelper->getPrice($item) ? $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) : 0, 2, ',', ''),
			'Tags'					=> $item->itemDescription->keywords,
			'EAN'					=> $this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode')),
			'LastUpdate'			=> $item->itemBase->lastUpdateTimestamp,
			'UnitPrice'				=> $this->elasticExportHelper->getBasePrice($item, $settings),
			'RetailerAttributes'	=> $item->itemBase->storeSpecial == 2 ? 'new-arrival' : '',
			'AvailableSizes'		=> [],
			'Color'					=> [],
		];

		return $data;
	}

	/**
	 * Get variation attributes.
	 * @param  Record   $item
	 * @param  KeyValue $settings
	 * @return array<string,string>
	 */
	private function getVariationAttributes(Record $item, KeyValue $settings):array
	{
		$variationAttributes = [];

		foreach($item->variationAttributeValueList as $variationAttribute)
		{
			$attributeValueName = $this->attributeValueNameRepository->findOne($variationAttribute->attributeValueId, $settings->get('lang'));

			if($attributeValueName instanceof AttributeValueName)
			{
				$amazonAttribute = $attributeValueName->attributeValue->attribute->amazonAttribute == 'Color';
				if($attributeValueName->attributeValue->attribute->amazonAttribute)
				{
					$variationAttributes[$attributeValueName->attributeValue->attribute->amazonAttribute][] = $attributeValueName->name;
				}
			}
		}

		return $variationAttributes;
	}

	/**
	 * Get item properties.
	 * @param 	Record $item
	 * @return array<string,string>
	 */
	protected function getItemPropertyList(Record $item):array
	{
		if(!array_key_exists($item->itemBase->id, $this->itemPropertyCache))
		{
			$characterMarketComponentList = $this->elasticExportHelper->getItemCharactersByComponent($item, 142.00);

			$list = [];

			if(count($characterMarketComponentList))
			{
				foreach($characterMarketComponentList as $data)
				{
					if((string) $data['characterValueType'] != 'file' && (string) $data['characterValueType'] != 'empty' && (string) $data['externalComponent'] != "0")
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

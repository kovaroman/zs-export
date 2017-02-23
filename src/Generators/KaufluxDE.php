<?php

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Item\Property\Contracts\PropertySelectionRepositoryContract;
use Plenty\Modules\Item\Property\Models\PropertySelection;
use Plenty\Modules\Helper\Contracts\UrlBuilderRepositoryContract;

/**
 * Class KaufluxDE
 * @package ElasticExport\Generators
 */
class KaufluxDE extends CSVGenerator
{
	const STATUS_VISIBLE = 0;
	const STATUS_LOCKED = 1;
	const STATUS_HIDDEN = 2;

	/*
     * @var
     */
    private $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

	/**
	 * PropertySelectionRepositoryContract $propertySelectionRepository
	 */
	private $propertySelectionRepository;

    /**
     * @var UrlBuilderRepositoryContract $urlBuilderRepository
     */
    private $urlBuilderRepository;

	/**
	 * @var array
	 */
	private $itemPropertyCache = [];

	/**
	 * @var array
	 */
	private $addedItems = [];

	/**
	 * @var array
	 */
	private $flags = [
		0 => '',
		1 => 'Sonderangebot',
		2 => 'Neuheit',
		3 => 'Top Artikel',
	];

    /**
     * KaufluxDE constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     * @param PropertySelectionRepositoryContract $propertySelectionRepository
     * @param UrlBuilderRepositoryContract $urlBuilderRepository
     */
    public function __construct(
		ElasticExportHelper $elasticExportHelper,
		ArrayHelper $arrayHelper,
		PropertySelectionRepositoryContract $propertySelectionRepository,
        UrlBuilderRepositoryContract $urlBuilderRepository
	)
    {
        $this->elasticExportHelper = $elasticExportHelper;
		$this->arrayHelper = $arrayHelper;
		$this->propertySelectionRepository = $propertySelectionRepository;
        $this->urlBuilderRepository = $urlBuilderRepository;
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

			$this->setDelimiter(";");

			$this->addCSVContent([
				'GroupID',
				'BestellNr',
				'EAN',
				'Hersteller',
				'BestandModus',
				'BestandAbsolut',
				'Liefertyp',
				'VersandKlasse',
				'Lieferzeit',
				'Umtausch',
				'Bezeichnung',
				'KurzText',
				'DetailText',
				'Keywords',
				'Bild1',
				'Bild2',
				'Bild3',
				'Gewicht',
				'Preis',
				'MwSt',
				'UVP',
				'Katalog1',
				'Flags',
				'LinkXS',
				'ExtLinkDetail',
				'Status',
				'FreeVar1',
				'FreeVar2',
				'FreeVar3',
				'InhaltMenge',
				'InhaltEinheit',
				'InhaltVergleich',
				'HerstellerArtNr',
			]);

			foreach($resultData as $item)
			{
				if(!$this->valid($item, $settings))
				{
					continue;
				}
                $basePriceList = $this->elasticExportHelper->getBasePriceList($item, $settings);

                $shippingCost = $this->elasticExportHelper->getShippingCost($item, $settings);
                if(is_null($shippingCost))
                {
                    $shippingCost = '';
                }

                $imageList = $this->elasticExportHelper->getImageListInOrder($item, $settings, 3, 'variationImages');

				$data = [
					'GroupID' 			=> $item->itemBase->id,
					'BestellNr' 		=> $this->elasticExportHelper->generateSku($item, 116.00, (string)$item->variationMarketStatus->sku),
					'EAN' 				=> $this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode')),
					'Hersteller' 		=> $this->elasticExportHelper->getExternalManufacturerName((int)$item->itemBase->producerId),
					'BestandModus' 		=> $this->config('stockCondition'),
					'BestandAbsolut' 	=> $this->getStock($item, $settings),
					'Liefertyp' 		=> 'V',
					'VersandKlasse' 	=> $shippingCost,
					'Lieferzeit' 		=> $this->elasticExportHelper->getAvailability($item, $settings, false),
					'Umtausch' 			=> $this->config('returnDays'),
					'Bezeichnung' 		=> $this->elasticExportHelper->getName($item, $settings) . ' ' . $item->variationBase->variationName,
					'KurzText' 			=> $this->elasticExportHelper->getPreviewText($item, $settings),
					'DetailText' 		=> $this->elasticExportHelper->getDescription($item, $settings) . ' ' . $this->getPropertyDescription($item, $settings),
					'Keywords' 			=> $item->itemDescription->keywords,
					'Bild1' 			=> count($imageList) > 0 && array_key_exists(0, $imageList) ? $imageList[0] : '',
					'Bild2' 			=> count($imageList) > 0 && array_key_exists(1, $imageList) ? $imageList[1] : '',
					'Bild3' 			=> count($imageList) > 0 && array_key_exists(2, $imageList) ? $imageList[2] : '',
					'Gewicht' 			=> $item->variationBase->weightG,
					'Preis' 			=> number_format((float)$this->elasticExportHelper->getPrice($item), 2, '.', ''),
					'MwSt' 				=> $item->variationRetailPrice->vatValue,
					'UVP' 				=> $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings),
					'Katalog1' 			=> $this->elasticExportHelper->getCategory((int)$item->variationStandardCategory->categoryId, $settings->get('lang'), 0, $settings->get('plentyId')),
					'Flags' 			=> in_array($item->itemBase->storeSpecial, $this->flags) ? $this->flags[$item->itemBase->storeSpecial] : '',
					'LinkXS' 			=> implode(', ', $this->getCrossSellingItems($item)),
					'ExtLinkDetail' 	=> $this->elasticExportHelper->getUrl($item, $settings),
					'Status' 			=> $this->getStatus($item),
					'FreeVar1' 			=> $item->itemBase->free1,
					'FreeVar2' 			=> $item->itemBase->free2,
					'FreeVar3' 			=> $item->itemBase->free3,
					'InhaltMenge' 		=> $basePriceList['lot'],
					'InhaltEinheit' 	=> $basePriceList['unit'], //TODO use Kauflux measurements
					'InhaltVergleich' 	=> '',
					'HerstellerArtNr' 	=> $item->variationBase->model,
				];

				$this->addCSVContent(array_values($data));
			}
		}
    }

	/**
	 * Get description of all correlated properties
	 * @param Record $item
	 * @param KeyValue $settings
	 * @return string
	 */
	private function getPropertyDescription(Record $item, KeyValue $settings):string
	{
		$properties = $this->getItemPropertyList($item);

		$propertyDescription = '';

		foreach($properties as $property)
		{
			$propertyDescription .= '<br/>' . $property;
		}

		return $propertyDescription;
	}

	/**
	 * Get item properties.
	 * @param 	Record $item
	 * @return array<string,string>
	 */
	private function getItemPropertyList(Record $item):array
	{
		if(!array_key_exists($item->itemBase->id, $this->itemPropertyCache))
		{
			$characterMarketComponentList = $this->elasticExportHelper->getItemCharactersByComponent($item, 116.00, 1);

			$list = [];

			if(count($characterMarketComponentList))
			{
				foreach($characterMarketComponentList as $data)
				{
					if((string) $data['characterValueType'] != 'file' && (string) $data['characterValueType'] != 'empty')
					{
						if((string) $data['characterValueType'] == 'selection')
						{
							$characterSelection = $this->propertySelectionRepository->findOne((int) $data['characterValue'], 'de');
							if($characterSelection instanceof PropertySelection)
							{
								$list[] = (string) $characterSelection->name;
							}
						}
						else
						{
							$list[] = (string) $data['characterValue'];
						}

					}
				}
			}

			$this->itemPropertyCache[$item->itemBase->id] = $list;
		}

		return $this->itemPropertyCache[$item->itemBase->id];
	}

	/**
	 * Get list of cross selling items.
	 * @param Record $item
	 * @return array<string>
	 */
	private function getCrossSellingItems(Record $item):array
	{
		$list = [];

		foreach($item->itemCrossSellingList as $itemCrossSelling)
		{
			$list[] = (string) $itemCrossSelling->crossItemId;
		}

		return $list;
	}

	/**
	 * Get status.
	 * @param  Record $item
	 * @return int
	 */
	private function getStatus(Record $item):int
	{
		if(!array_key_exists($item->itemBase->id, $this->addedItems))
		{
			$this->addedItems[$item->itemBase->id] = $item->itemBase->id;

			return self::STATUS_VISIBLE;
		}

		return self::STATUS_HIDDEN;
	}

	/**
	 * Get stock.
	 * @param Record $item
	 * @param KeyValue $settings
	 * @return int
	 */
	private function getStock(Record $item, KeyValue $settings):int
	{
		$stock = $item->variationStock->stockNet;

		if ($item->variationBase->limitOrderByStockSelect == 0 || $this->config('stockCondition') == 'N')
		{
			$stock = 100;
		}

		return (int) $stock;
	}

	/**
	 * Get kauflux configuration.
	 * @param  string $key
	 * @return string
	 */
	private function config(string $key):string
	{
		$config = $this->elasticExportHelper->getConfig('plenty.market.kauflux');

		if(is_array($config) && array_key_exists($key, $config))
		{
			return (string) $config[$key];
		}

		return '';
	}

	/**
	 * Check if stock available.
	 * @param  Record $item
	 * @param  KeyValue $settings
	 * @return bool
	 */
	private function valid(Record $item, KeyValue $settings):bool
	{
		$stock = $item->variationStock->stockNet;

		if ($item->variationBase->limitOrderByStockSelect == 0 || $this->config('stockCondition') == 'N')
		{
			$stock = 100;
		}

		if($this->config('stockCondition') != 'N' && $stock <= 0)
		{
			return false;
		}

		return true;
	}
}

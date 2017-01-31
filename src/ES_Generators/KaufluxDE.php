<?php

namespace ElasticExport\ES_Generators;

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
     * @var array $idlVariations
     */
    private $idlVariations = array();

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
     * @param array $resultData
     * @param array $formatSettings
     */
    protected function generateContent($resultData, array $formatSettings = [])
    {
        if(is_array($resultData) && count($resultData['documents']) > 0)
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

            //Create a List of all VariationIds
            $variationIdList = array();
            foreach($resultData['documents'] as $variation)
            {
                $variationIdList[] = $variation['id'];
            }

            //Get the missing fields in ES from IDL
            if(is_array($variationIdList) && count($variationIdList) > 0)
            {
                /**
                 * @var \ElasticExport\ES_IDL_ResultList\KaufluxDE $idlResultList
                 */
                $idlResultList = pluginApp(\ElasticExport\ES_IDL_ResultList\KaufluxDE::class);
                $idlResultList = $idlResultList->getResultList($variationIdList, $settings);
            }

            //Creates an array with the variationId as key to surpass the sorting problem
            if(isset($idlResultList) && $idlResultList instanceof RecordList)
            {
                $this->createIdlArray($idlResultList);
            }

			foreach($resultData['documents'] as $item)
			{
				if(!$this->valid($item))
				{
					continue;
				}

				$basePriceList = $this->elasticExportHelper->getEsBasePriceList($item, $this->idlVariations[$item['id']]['variationRetailPrice.price']);

                $shippingCost = $this->elasticExportHelper->getEsShippingCost($item['data']['item']['id'], $settings);
                if(is_null($shippingCost))
                {
                    $shippingCost = '';
                }

                $imageList = $this->elasticExportHelper->getEsImageListInOrder($item, $settings, 3, 'variationImages');

				$data = [
					'GroupID' 			=> $item['data']['item']['id'],
					'BestellNr' 		=> $this->elasticExportHelper->generateEsSku($item['id'], 116, 0, (string)$item['data']['skus']['sku']),
					'EAN' 				=> $this->elasticExportHelper->getEsBarcodeByType($item, $settings->get('barcode')),
					'Hersteller' 		=> $this->elasticExportHelper->getExternalManufacturerName((int)$item['data']['item']['manufacturer']['id']),
					'BestandModus' 		=> $this->config('stockCondition'),
					'BestandAbsolut' 	=> $this->getStock($item),
					'Liefertyp' 		=> 'V',
					'VersandKlasse' 	=> $shippingCost,
					'Lieferzeit' 		=> $this->elasticExportHelper->getEsAvailability($item, $settings, false),
					'Umtausch' 			=> $this->config('returnDays'),
					'Bezeichnung' 		=> $this->elasticExportHelper->getEsName($item, $settings), //. ' ' . $item->variationBase->variationName, todo maybe add the attribute value name
					'KurzText' 			=> $this->elasticExportHelper->getEsPreviewText($item, $settings),
					'DetailText' 		=> $this->elasticExportHelper->getEsDescription($item, $settings) . ' ' . $this->getPropertyDescription($item),
					'Keywords' 			=> $item['data']['texts']['keywords'],
					'Bild1' 			=> count($imageList) > 0 && array_key_exists(0, $imageList) ? $imageList[0] : '',
					'Bild2' 			=> count($imageList) > 0 && array_key_exists(1, $imageList) ? $imageList[1] : '',
					'Bild3' 			=> count($imageList) > 0 && array_key_exists(2, $imageList) ? $imageList[2] : '',
					'Gewicht' 			=> $item['data']['variation']['weightG'],
					'Preis' 			=> number_format((float)$this->idlVariations[$item['id']]['variationRetailPrice.price'], 2, '.', ''),
					'MwSt' 				=> $this->idlVariations[$item['id']]['variationRetailPrice.vatValue'],
					'UVP' 				=> $this->elasticExportHelper->getEsRecommendedRetailPrice($item, $settings),
					'Katalog1' 			=> $this->elasticExportHelper->getCategory((int)$item['data']['defaultCategories'][0]['id'], $settings->get('lang'), 0, $settings->get('plentyId')),
					'Flags' 			=> in_array($item['data']['item']['storeSpecial'], $this->flags) ? $this->flags[$item['data']['item']['storeSpecial']] : '',
					'LinkXS' 			=> implode(', ', $this->getCrossSellingItems($item)),
					'ExtLinkDetail' 	=> $this->elasticExportHelper->getEsUrl($item, $settings),
					'Status' 			=> $this->getStatus($item),
					'FreeVar1' 			=> $item['data']['item']['free1'],
					'FreeVar2' 			=> $item['data']['item']['free2'],
					'FreeVar3' 			=> $item['data']['item']['free3'],
					'InhaltMenge' 		=> $basePriceList['lot'],
					'InhaltEinheit' 	=> $basePriceList['unit'], //TODO use Kauflux measurements
					'InhaltVergleich' 	=> '',
					'HerstellerArtNr' 	=> $item['data']['variation']['model'],
				];

				$this->addCSVContent(array_values($data));
			}
		}
    }

	/**
	 * Get description of all correlated properties
	 * @param array $item
	 * @return string
	 */
	private function getPropertyDescription($item):string
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
	 * @param 	array $item
	 * @return array<string,string>
	 */
	private function getItemPropertyList($item):array
	{
		if(!array_key_exists($item['data']['item']['id'], $this->itemPropertyCache))
		{
			$characterMarketComponentList = $this->elasticExportHelper->getEsItemCharactersByComponent($item, 116.00, 1);

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

			$this->itemPropertyCache[$item['data']['item']['id']] = $list;
		}

		return $this->itemPropertyCache[$item['data']['item']['id']];
	}

	/**
	 * Get list of cross selling items.
	 * @param array $item
	 * @return array<string>
	 */
	private function getCrossSellingItems($item):array
	{
		$list = [];

		foreach($this->idlVariations[$item['id']]['itemCrossSellingList'] as $itemCrossSelling)
		{
			$list[] = (string) $itemCrossSelling->crossItemId;
		}

		return $list;
	}

	/**
	 * Get status.
	 * @param  array $item
	 * @return int
	 */
	private function getStatus($item):int
	{
		if(!array_key_exists($item['data']['item']['id'], $this->addedItems))
		{
			$this->addedItems[$item['data']['item']['id']] = $item['data']['item']['id'];

			return self::STATUS_VISIBLE;
		}

		return self::STATUS_HIDDEN;
	}

	/**
	 * Get stock.
	 * @param array $item
	 * @return int
	 */
	private function getStock($item):int
	{
		$stock = $this->idlVariations['variationStock.stockNet'];

		if ($item['data']['variation']['stockLimitation'] == 0 || $this->config('stockCondition') == 'N')
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
	 * @param  array $item
	 * @return bool
	 */
	private function valid($item):bool
	{
		$stock = $this->idlVariations[$item['id']]['variationStock.stockNet'];

		if ($item['data']['variation']['stockLimitation'] == 0 || $this->config('stockCondition') == 'N')
		{
			$stock = 100;
		}

		if($this->config('stockCondition') != 'N' && $stock <= 0)
		{
			return false;
		}

		return true;
	}

    /**
     * @param RecordList $idlResultList
     */
    private function createIdlArray($idlResultList)
    {
        if($idlResultList instanceof RecordList)
        {
            foreach($idlResultList as $idlVariation)
            {
                if($idlVariation instanceof Record)
                {
                    $this->idlVariations[$idlVariation->variationBase->id] = [
                        'itemBase.id' => $idlVariation->itemBase->id,
                        'variationBase.id' => $idlVariation->variationBase->id,
                        'itemCrossSellingList' => $idlVariation->itemCrossSellingList,
                        'itemPropertyList' => $idlVariation->itemPropertyList,
                        'variationStock.stockNet' => $idlVariation->variationStock->stockNet,
                        'variationRetailPrice.price' => $idlVariation->variationRetailPrice->price,
                        'variationRetailPrice.vatValue' => $idlVariation->variationRetailPrice->vatValue,
                        'variationRecommendedRetailPrice.price' => $idlVariation->variationRecommendedRetailPrice->price,
                    ];
                }
            }
        }
    }

}

<?php

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;

class Check24DE extends CSVGenerator
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
     * Billiger constructor.
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
	protected function generateContent($resultData, array $formatSettings = [])
	{
		if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter("|");

			$this->addCSVContent([
					'id',
					'manufacturer',
					'mpnr',
					'ean',
					'name',
					'description',
					'category_path',
					'price',
					'price_per_unit',
					'link',
					'image_url',
					'delivery_time',
					'delivery_cost',
					'pzn',
					'stock',
					'weight',
			]);

			foreach($resultData as $item)
			{
				$variationName = $this->elasticExportHelper->getAttributeValueSetShortFrontendName($item, $settings);
				
				$data = [
					'id' 				=> $this->getSku($item),
					'manufacturer' 		=> $item->itemBase->producer,
					'mpnr' 				=> $item->variationBase->model,
					'ean' 				=> $this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode')),
					'name' 				=> $this->elasticExportHelper->getName($item, $settings) . (strlen($variationName) ? ' ' . $variationName : ''),
					'description' 		=> $this->elasticExportHelper->getDescription($item, $settings),
					'category_path' 	=> $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
					'price' 			=> number_format($this->elasticExportHelper->getPrice($item), 2, '.', ''),
					'price_per_unit'	=> $this->elasticExportHelper->getBasePrice($item, $settings),
					'link' 				=> $this->elasticExportHelper->getUrl($item, $settings, true, false),
					'image_url'			=> $this->elasticExportHelper->getMainImage($item, $settings),
					'delivery_time' 	=> $this->elasticExportHelper->getAvailability($item, $settings, false),
					'dellvery_cost' 	=> number_format($this->elasticExportHelper->getShippingCost($item, $settings), 2, ',', ''),
					'pzn' 				=> '',
					'stock' 			=> $item->variationStock->stockNet,
					'weight' 			=> $item->variationBase->weightG,	
				];

				$this->addCSVContent(array_values($data));
			}
		}
	}

	/**
	 * Get the SKU.
	 * @param  Record $item
	 * @return string
	 */
	private function getSku(Record $item):string
	{
		return (string) $this->elasticExportHelper->generateSku($item, 150, (string)$item->variationMarketStatus->sku);
	}
}

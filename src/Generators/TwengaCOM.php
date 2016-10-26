<?php
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;


class TwengaCOM extends CSVGenerator
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
     * TwengaCOM constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ElasticExportHelper $elasticExportHelper, ArrayHelper $arrayHelper)
    {
        $this->elasticExportHelper = $elasticExportHelper;
        $this->arrayHelper = $arrayHelper;
    }

    /**
     * @param RecordList $resultData
     */
    protected function generateContent($resultData, array  $formatSettings = [])
	{
		if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(";");

			$this->addCSVContent([
                        // Compulsory fields
                        'product_url',
                        'designation',
                        'price',
                        'category',
                        'image_url',
                        'description',
                        'regular_price',
                        'shipping_cost',
                        // Optional fields
                        'merchant_id',
                        'manufacturer_id',
                        'in_stock',
                        'stock_detail',
                        'condition',
                        'upc_ean',
                        'isbn',
                        'brand',
                    ]);
            $itemCondition =    [
                                    0 => '0', 	// plenty condition: NEU
                                    1 => '1', 	// plenty condition: GEBRAUCHT
                                    2 => '0', 	// plenty condition: NEU & OVP
                                    3 => '0', 	// plenty condition: NEU mit Etikett
                                    4 => '1', 	// plenty condition: B-WARE
                                ];

			foreach($resultData as $item)
			{
                $rrp = $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) > $this->elasticExportHelper->getPrice($item) ? $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) : '';

                $shippingCost = $this->elasticExportHelper->getShippingCost($item, $settings);
                if(!is_null($shippingCost))
                {
                    $shippingCost = number_format($shippingCost, 2, '.', '');
                }
                else
                {
                    $shippingCost = '';
                }

				$data = [
                    'product_url'       => $this->elasticExportHelper->getUrl($item, $settings, true, false),
                    'designation'       => $this->elasticExportHelper->getName($item, $settings),
                    'price'             => number_format($this->elasticExportHelper->getPrice($item), 2, '.', ''),
                    'category'          => $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
                    'image_url'         => $this->elasticExportHelper->getMainImage($item, $settings),
                    'description'       => $this->elasticExportHelper->getDescription($item, $settings, 256),
                    'regular_price'     => $rrp,
                    'shipping_cost'     => $shippingCost,
                    'merchant_id'       => $item->variationBase->customNumber,
                    'manufacturer_id'   => $item->variationBase->model,
                    'in_stock'          => $item->variationStock->stockNet > 0 ? 'Y' : 'N',
                    'stock_detail'      => $item->variationStock->stockNet,
                    'condition'         => $itemCondition[(int)$item->itemBase->condition],
                    'upc_ean'           => $this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode')),
                    'isbn'              => $this->elasticExportHelper->getBarcodeByType($item, ElasticExportHelper::BARCODE_ISBN),
                    'brand'             => $this->elasticExportHelper->getExternalManufacturerName($item->itemBase->producerId)
				];

				$this->addCSVContent(array_values($data));
			}
        }
    }
}

<?php
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;


class ZanoxDE extends CSVGenerator
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
     * ZanoxDE constructor.
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
     * @param array $formatSettings
     */
    protected function generateContent($resultData, array $formatSettings = [])
	{
		if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter(";");

			$this->addCSVContent([
                        'prod_number',
                        'prod_name',
                        'prod_price',
                        'currency_symbol',
                        'category',
                        'prod_description',
                        'prod_description_long',
                        'img_small',
                        'img_medium',
                        'img_large',
                        'manufacturer',
                        'prod_url',
                        'prod_ean',
                        'shipping_costs',
                        'base_price',
                        'base_price_amount',
                        'base_price_unit',
                    ]);

			foreach($resultData as $item)
			{
                $basePriceList = $this->elasticExportHelper->getBasePriceList($item, $settings);
                $shippingCost = $this->elasticExportHelper->getShippingCost($item, $settings);

                if(!is_null($shippingCost))
                {
                    $shippingCost = number_format((float)$shippingCost, 2, '.', '');
                }
                else
                {
                    $shippingCost = '';
                }

				$data = [
                    'prod_number'           => $item->itemBase->id,
                    'prod_name'             => strip_tags(html_entity_decode($this->elasticExportHelper->getName($item, $settings))),
                    'prod_price'            => number_format((float)$this->elasticExportHelper->getPrice($item), 2, '.', ''),
                    'currency_symbol'       => $item->variationRetailPrice->currency,
                    'category'              => $this->elasticExportHelper->getCategory((int)$item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
                    'prod_description'      => strip_tags(html_entity_decode($this->elasticExportHelper->getPreviewText($item, $settings, 256))),
                    'prod_description_long' => strip_tags(html_entity_decode($this->elasticExportHelper->getDescription($item, $settings, 256))),
                    'img_small'             => $this->elasticExportHelper->getMainImage($item, $settings, 'preview'),
                    'img_medium'            => $this->elasticExportHelper->getMainImage($item, $settings, 'middle'),
                    'img_large'             => $this->elasticExportHelper->getMainImage($item, $settings, 'normal'),
                    'manufacturer'          => $this->elasticExportHelper->getExternalManufacturerName((int)$item->itemBase->producerId),
                    'prod_url'              => $this->elasticExportHelper->getUrl($item, $settings, true, false),
                    'prod_ean'              => $item->variationBarcode->code,
                    'shipping_costs'        => $shippingCost,
                    'base_price'            => $this->elasticExportHelper->getBasePrice($item, $settings, '/', false, true, '', 0.0, false),
                    'base_price_amount'     => $basePriceList['lot'],
                    'base_price_unit'       => $basePriceList['unit']
				];

				$this->addCSVContent(array_values($data));
			}
        }
    }
}

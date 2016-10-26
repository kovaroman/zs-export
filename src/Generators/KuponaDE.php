<?php

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Helper\Models\KeyValue;

class KuponaDE extends CSVGenerator
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
     * KuponaDE constructor.
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
                    'prod_price_old',
                    'currency_symbol',
                    'prod_url',
                    'category',
                    'category_url',
                    'valid_from_date',
                    'valid_to_date',
                    'prod_description',
                    'prod_description_long',
                    'img_small',
                    'img_medium',
                    'img_large',
                    'ean_code',
                    'versandkosten',
                    'lieferzeit',
                    'platform',
                    'grundpreis',

                    ]);

			foreach($resultData as $item)
			{
                $rrp = $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) > $this->elasticExportHelper->getPrice($item) ? $this->elasticExportHelper->getRecommendedRetailPrice($item, $settings) : '';

                $deliveryCost = $this->elasticExportHelper->getShippingCost($item, $settings);
                if(!is_null($deliveryCost))
                {
                    $deliveryCost = number_format($deliveryCost, 2, ',', '');
                }
                else
                {
                    $deliveryCost = '';
                }

				$data = [
                    'prod_number'           => $item->itemBase->id,
                    'prod_name'             => $this->elasticExportHelper->getName($item, $settings),
                    'prod_price'            => number_format($this->elasticExportHelper->getPrice($item), 2, '.', ''),
                    'prod_price_old'        => $rrp,
                    'currency_symbol'       => $item->variationRetailPrice->currency,
                    'prod_url'              => $this->elasticExportHelper->getUrl($item, $settings, true, false),
                    'category'              => $this->elasticExportHelper->getCategory($item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
                    'category_url'          => '',
                    'valid_from_date'       => '',
                    'valid_to_date'         => '',
                    'prod_description'      => $this->elasticExportHelper->getPreviewText($item, $settings, 256),
                    'prod_description_long' => $this->elasticExportHelper->getDescription($item, $settings, 256),
                    'img_small'             => $this->getImages($item, $settings, ';', 'preview'),
                    'img_medium'            => $this->getImages($item, $settings, ';', 'middle'),
                    'img_large'             => $this->getImages($item, $settings, ';', 'normal'),
                    'ean_code'              => $item->variationBarcode->code,
                    'versandkosten'         => $deliveryCost,
                    'lieferzeit'            => $this->elasticExportHelper->getAvailability($item, $settings),
                    'platform'              => '',
                    'grundpreis'            => $this->elasticExportHelper->getBasePrice($item, $settings),

				];

				$this->addCSVContent(array_values($data));
			}
        }
    }

    /**
     * Get images.
     * @param  Record   $item
     * @param  KeyValue $settings
     * @param  string   $separator  = ','
     * @param  string   $imageType  = 'normal'
     * @return string
     */
    public function getImages(Record $item, KeyValue $settings, string $separator = ',', string $imageType = 'normal'):string
    {
        $list = $this->elasticExportHelper->getImageList($item, $settings, $imageType);

        if(count($list))
        {
            return implode($separator, $list);
        }

        return '';
    }
}

<?php

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;


class KelkooBasicDE extends CSVGenerator
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
     * KelkooBasicDE constructor.
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

			$this->setDelimiter(" ");

			$this->addCSVContent([
					'url',
                    'title',
                    'description',
                    'price',
                    'offerid',
                    'image',
                    'availability',
                    'deliverycost',
                    'deliveryTime',
                    'unitaryPrice',
                    'ean',
                    'ecotax',

			]);

			foreach($resultData as $item)
			{
			    $deliveryCost = $this->elasticExportHelper->getShippingCost($item, $settings);

                if(!is_null($deliveryCost))
                {
                    $deliveryCost = number_format((float)$deliveryCost, 2, ',', '');
                }
                else
                {
                    $deliveryCost = '';
                }
			    
				$data = [
                    'url' 		    => $this->elasticExportHelper->getUrl($item, $settings, true, false),
                    'title' 		=> $this->elasticExportHelper->getName($item, $settings, 80),
                    'description'   => $this->elasticExportHelper->getDescription($item, $settings, 160),
                    'price' 	    => number_format((float)$this->elasticExportHelper->getPrice($item), 2, ',', ''),
                    'offerid'       => $item->variationBase->id,
                    'image'		    => $this->elasticExportHelper->getMainImage($item, $settings),
                    'availability'  => $this->elasticExportHelper->getAvailability($item, $settings, false),
					'deliverycost' 	=> $deliveryCost,
                    'deliveryTime' 	=> $this->elasticExportHelper->getAvailability($item, $settings),
                    'unitaryPrice'  => $this->elasticExportHelper->getBasePrice($item, $settings),
                    'ean'           => $item->variationBarcode->code,
                    'ecotax'        => ''
				];

				$this->addCSVContent(array_values($data));
			}
        }
    }
}

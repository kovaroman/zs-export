<?php

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;

class GuenstigerDE extends CSVGenerator
{
    const DELIMITER = '|';
	/*
     * @var ElasticExportHelper
     */
    private $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

	/**
     * Guenstiger constructor.
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

			$this->setDelimiter(self::DELIMITER);

			$this->addCSVContent([
                'bezeichnung',
                'preis',
                'deeplink',
                'ean',
                'beschreibung',
                'bilderlink',
                'lieferzeiten',
                'lieferkosten',
	            'grundpreis',
			]);

			foreach($resultData as $item)
			{
                $shippingCost = $this->elasticExportHelper->getShippingCost($item, $settings);
                if(!is_null($shippingCost))
                {
                    $shippingCost = number_format($shippingCost, 2, ',', '');
                }
                else
                {
                    $shippingCost = '';
                }

				$data = [
					'bezeichnung'      => $this->elasticExportHelper->getName($item, $settings, 256),
					'preis' 	       => number_format($this->elasticExportHelper->getPrice($item), 2, '.', ''),
					'deeplink' 		   => $this->elasticExportHelper->getUrl($item, $settings, true, false),
					'ean' 		       => $this->elasticExportHelper->getBarcodeByType($item, $settings->get('barcode')),
					'beschreibung' 	   => $this->elasticExportHelper->getDescription($item, $settings, 256),
					'bilderlink'	   => $this->elasticExportHelper->getMainImage($item, $settings),
					'lieferzeiten' 	   => $this->elasticExportHelper->getAvailability($item, $settings),
					'lieferkosten' 	   => $shippingCost,
	                'grundpreis' 	   => $this->elasticExportHelper->getBasePrice($item, $settings),
				];

				$this->addCSVContent(array_values($data));
			}
		}
	}
}

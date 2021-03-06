<?php
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;

class ShoppingCOM extends CSVGenerator
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
     * Shopping constructor.
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

			$this->setDelimiter(",");

			$this->addCSVContent([
					'Händler-SKU',
					'Hersteller',
					'EAN',
					'Produktname',
					'Produktbeschreibung',
					'Preis',
					'Produkt-URL',
					'Produktbild-URL',
					'Kategorie',
					'Verfügbar',
					'Verfügbarkeitdetails',
					'Versand: Landtarif',
					'Produktgewicht',
					'Produkttyp',
					'Grundpreis',
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
					'Händler-SKU' 			=> $item->itemBase->id,
					'Hersteller' 			=> $this->elasticExportHelper->getExternalManufacturerName((int)$item->itemBase->producerId),
					'EAN' 					=> $item->variationBarcode->code,
					'Produktname' 			=> $this->elasticExportHelper->getName($item, $settings),
					'Produktbeschreibung' 	=> $this->elasticExportHelper->getDescription($item, $settings),
					'Preis' 				=> number_format((float)$this->elasticExportHelper->getPrice($item), 2, ',', ''),
					'Produkt-URL' 			=> $this->elasticExportHelper->getUrl($item, $settings, true, false),
	                'Produktbild-URL' 		=> $this->elasticExportHelper->getMainImage($item, $settings),
					'Kategorie'				=> $this->elasticExportHelper->getCategory((int)$item->variationStandardCategory->categoryId, $settings->get('lang'), $settings->get('plentyId')),
					'Verfügbar' 			=> 'Ja',
					'Verfügbarkeitdetails' 	=> $this->elasticExportHelper->getAvailability($item, $settings),
	                'Versand: Landtarif' 	=> $deliveryCost,
                    'Produktgewicht'        => $item->variationBase->weightG,
	                'Produkttyp' 			=> $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, 'product_type'),
					'Grundpreis' 			=> $this->elasticExportHelper->getBasePrice($item, $settings),
				];

				$this->addCSVContent(array_values($data));
			}
		}
	}
}

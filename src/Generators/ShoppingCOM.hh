<?hh // strict
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
    private ElasticExportHelper $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
	private ArrayHelper $arrayHelper;

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
	 * @param mixed $resultData
	 */
	protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
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
				$data = [
					'Händler-SKU' 			=> $item->itemBase->id,
					'Hersteller' 			=> $item->itemBase->producer,
					'EAN' 					=> $item->variationBarcode->code,
					'Produktname' 			=> $this->elasticExportHelper->getName($item, $settings),
					'Produktbeschreibung' 	=> $this->elasticExportHelper->getDescription($item, $settings),
					'Preis' 				=> number_format($this->elasticExportHelper->getPrice($item, $settings), 2, ',', ''),
					'Produkt-URL' 			=> $this->elasticExportHelper->getUrl($item, $settings, true, false),
	                'Produktbild-URL' 		=> $this->elasticExportHelper->getMainImage($item, $settings),
					'Kategorie'				=> $this->elasticExportHelper->getCategory($item, $settings),
					'Verfügbar' 			=> 'Ja',
					'Verfügbarkeitdetails' 	=> $this->elasticExportHelper->getAvailability($item, $settings),
	                'Versand: Landtarif' 	=> number_format($this->elasticExportHelper->getShippingCost($item, $settings), 2, ',', ''),
                    'Produktgewicht'        => $item->variationBase->weightG,
	                'Produkttyp' 			=> $this->elasticExportHelper->getItemCharacterByBackendName($item, $settings, 'product_type'),
					'Grundpreis' 			=> $this->elasticExportHelper->getBasePrice($item, $settings),
				];

				$this->addCSVContent(array_values($data));
			}
		}
	}
}

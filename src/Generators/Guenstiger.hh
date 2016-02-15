<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;

class Guenstiger extends CSVGenerator
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
	 * @param mixed $resultData
	 */
	protected function generateContent(mixed $resultData, array<FormatSetting> $formatSettings = []):void
	{
		if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			$this->setDelimiter('|');

			$this->addCSVContent([
                'bezeichnung',
                'preis',
                'deeplink',
                'beschreibung',
                'bilderlink',
                'lieferzeiten',
                'lieferkosten',
                'ean',
	            'grundpreis',
			]);

			foreach($resultData as $item)
			{
				$data = [
					'bezeichnung'      => $this->elasticExportHelper->getName($item, $settings, 256),
					'preis' 	       => number_format($this->elasticExportHelper->getPrice($item, $settings), 2, '.', ''),
					'deeplink' 		   => $this->elasticExportHelper->getUrl($item, $settings, true, false),
					'ean' 		       => $item->variationBarcode->code,
					'beschreibung' 	   => $this->elasticExportHelper->getDescription($item, $settings, 256),
					'bilderlink'	   => $this->elasticExportHelper->getMainImage($item, $settings),
					'lieferzeiten' 	   => $this->elasticExportHelper->getAvailability($item, $settings),
					'lieferkosten' 	   => number_format($this->elasticExportHelper->getShippingCost($item, $settings), 2, ',', ''),
	                'grundpreis' 	   => $this->elasticExportHelper->getBasePrice($item, $settings),
				];

				$this->addCSVContent(array_values($data));
			}
		}
	}
}

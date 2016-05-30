<?hh // strict
namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\CSVGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;

class ShippingProfiles extends CSVGenerator
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
	 * @var int
	 */
	private int $columns = 5;

	/**
	 * ShippingProfiles constructor.
	 * @param ElasticExportHelper $elasticExportHelper
	 * @param ArrayHelper $arrayHelper
	 */
	public function __construct(
		ElasticExportHelper $elasticExportHelper,
		ArrayHelper $arrayHelper		
	)
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

			$this->setDelimiter(";");
			$this->setEnclosure("'");

			$rows = [];

			foreach($resultData as $item)
			{
				$row = [
					'item_id' => $item->itemBase->id,
				];

				foreach($this->getShippingSupportIds($item) as $key => $id)
				{
					$row['parcel_service_preset_id' . (string) ($key+1)] = $id;
				}

				$rows[] = $row;
			}

			$this->addCSVContent($this->head());

			foreach($rows as $row)
			{
				$this->addCSVContent($this->row(array_values($row)));
			}
		}
	}

	/**
	 * Get the head row.
	 * @return array<mixed>
	 */
	private function head():array<mixed>
	{
		$row = ['item_id'];

		for($i = 1; $i <= $this->columns; $i++)
		{
			$row[] = 'parcel_service_preset_id' . (string) $i;
		}

		return $row;
	}

	/**
	 * Get row.
	 * @param  <array<string,mixed> $row
	 * @return array<mixed>
	 */
	private function row(array<mixed> $row):array<mixed>
	{
		for($i = count($row); $i <= $this->columns; $i++)
		{
			$row[] = '';
		}

		return $row;
	}

	/**
	 * Get list of supported shipping profile ids for the given item.
	 * @param  Record $item
	 * @return array<int,int>
	 */
	private function getShippingSupportIds(Record $item):array<int>
	{
		$ids = [];

		foreach($item->itemShippingProfilesList as $itemShippingProfile)
		{
			$ids[] = $itemShippingProfile->id;
		}

		$this->maxColumns(count($ids));

		return $ids;
	}

	/**
	 * Update maximum number of available columns.
	 * @return void
	 */
	private function maxColumns(int $columns):void
	{
		$this->columns = $columns > $this->columns ? $columns : $this->columns;
	}
}
